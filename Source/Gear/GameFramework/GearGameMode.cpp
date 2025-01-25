// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearGameMode.h"
#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearGameState.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearBuilderPawn.h"
#include "Vehicle/GearVehicle.h"
#include "Vehicle/VehicleCamera.h"

#include "GameSystems/Checkpoint.h"
#include "GameSystems/VehicleStart.h"
#include "GameSystems/GearStatics.h"

#include "Placeable/GearPlaceable.h"
#include "Placeable/GearRoadModule.h"
#include "Placeable/GearHazard.h"
#include "Placeable/SpawnableSocket.h"
#include "Placeable/HazardSocketComponent.h"
#include "Placeable/SelectionPlatform.h"

#include "Utils/GameVariablesBFL.h"
#include "kismet/GameplayStatics.h"

#define PREVIEW_MODULE_TAG TEXT("PreviewMoudle")
#define SPAWNED_ACTOR_TAG "Spawned"

AGearGameMode::AGearGameMode()
{
	PlayerControllerClass = AGearPlayerController::StaticClass();
	GameStateClass = AGearGameState::StaticClass();
	PlayerStateClass = AGearPlayerState::StaticClass();

	bUseSeamlessTravel = true;

	CheckpointDistance = 1000.0f;	
}

void AGearGameMode::BeginPlay()
{
	Super::BeginPlay();

	GearGameState = GetGameState<AGearGameState>();
	check(IsValid(GearGameState));
	SetGearMatchState(EGearMatchState::WaitingForPlayerToJoin);

	bool bFailed = false;

	if (AvaliablePlaceables.Num() == 0)
	{
		UE_LOG(LogGameMode, Error, TEXT("Not enough placeable found"));
		bFailed = true;
	}

	if (!GearGameState->FindStartRoadModuleAndAddToStack())
	{
		UE_LOG(LogGameMode, Error, TEXT("finding first raod module failed"));
		bFailed = true;
	}

	if (!GearGameState->FindStartCheckpointAndAddToStack())
	{
		UE_LOG(LogGameMode, Error, TEXT("finding start checkpoint failed"));
		bFailed = true;
	}

	if (bFailed)
	{
		AbortMatch();
	}
}

void AGearGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetGearMatchState() == EGearMatchState::WaitingForPlayerToJoin)
	{
		if (!GearGameState->bEveryPlayerReady && CheckIsEveryPlayerReady())
		{
			AllPlayerJoined();
		}
	}

	else if (GetGearMatchState() == EGearMatchState::SelectingPlaceables)
	{
		if (IsEveryPlayerSelectedPlaceables())
		{
			StartPlaceing(true);
		}
	}

	else if (GetGearMatchState() == EGearMatchState::Placing)
	{
		if (IsEveryPlayerPlaced())
		{
			EndPlaceing();
		}
	}

	else if (GetGearMatchState() == EGearMatchState::Racing)
	{
		RacingTick(DeltaSeconds);
	}

	if (ShouldAbort())
	{
		AbortMatch();
	}
}


void AGearGameMode::RacingTick(float DeltaSeconds)
{
	const bool bInvincibilityFinished = GetWorld()->GetTimeSeconds() - GearGameState->LastCheckpointStartTime > UGameVariablesBFL::GV_InvincibilityDuration();

	for (int i = GearGameState->Vehicles.Num() - 1; i >= 0; i--)
	{
		AGearVehicle* Vehicle = GearGameState->Vehicles[i];

		if (IsValid(Vehicle))
		{
			EElimanationReason ElimanationReason;
			bool bShouldVehicleDie = ShouldVehicleDie(Vehicle, ElimanationReason);

			if (bShouldVehicleDie)
			{
				GearGameState->BroadcastEliminationEvent_Multi(Vehicle->GetPlayerState<AGearPlayerState>(), ElimanationReason, GearGameState->GetServerWorldTimeSeconds(), GearGameState->GetDrivingPlayerCount() - 1); 
				DestroyVehicle(Vehicle);
			}

			else if (bInvincibilityFinished && Vehicle->HasInvincibility() && !Vehicle->IsSpectating() && Vehicle->CanRemoveInvincibility())
			{
				Vehicle->RemoveInvincibility();
				Vehicle->OnRep_GrantedInvincibility();
			}
		}
	}

	bool bEveryPlayerEliminated = IsEveryPlayerEliminated();

	if (bEveryPlayerEliminated && !RacingEndDelayTimerHandle.IsValid())
	{
		for (int i = GearGameState->Vehicles.Num() - 1; i >= 0; i--)
		{
			AGearVehicle* Vehicle = GearGameState->Vehicles[i];
			Vehicle->Destroy();
		}
		GearGameState->Vehicles.Empty(4);

		ACheckpoint* NextCheckpoint = GearGameState->GetNextFurthestReachedCheckpoint();
		const bool bRaceFinished = !IsValid(NextCheckpoint);

		if (bRaceFinished)
		{
			GetWorld()->GetTimerManager().SetTimer(RacingEndDelayTimerHandle, FTimerDelegate::CreateUObject(this, &AGearGameMode::StartScoreboard), UGameVariablesBFL::GV_RacingEndDelay(), false);
		}
		else
		{
			GearGameState->ClearOccupiedVehicleStarts();
			AGearVehicle* NullVehicle = nullptr;
			GetWorld()->GetTimerManager().SetTimer(RacingEndDelayTimerHandle, FTimerDelegate::CreateUObject(this, &AGearGameMode::StartRacingAtCheckpoint, NextCheckpoint, NullVehicle), UGameVariablesBFL::GV_RacingEndDelay(), false);
		}
	}
}

bool AGearGameMode::ShouldAbort()
{
	return NumTravellingPlayers == 0 && NumPlayers < 2;
}

void AGearGameMode::SpawnSelectionPlatform()
{
	FVector SpawnLocation = FVector
		( (GearGameState->WorldMin.X + GearGameState->WorldMax.X) / 2
		, (GearGameState->WorldMin.Y + GearGameState->WorldMax.Y) / 2
		, GearGameState->WorldMax.Z + 2000.0f);

	SelectionPlatform = GetWorld()->SpawnActor<ASelectionPlatform>(SelectionPlatformClass, SpawnLocation, FRotator::ZeroRotator);
}

void AGearGameMode::SpawnNewBuilderPawns()
{
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AGearPlayerController* PlayerController = Cast<AGearPlayerController>(*It);
		if (IsValid(PlayerController))
		{
			AGearBuilderPawn* BuilderPawn = GetWorld()->SpawnActor<AGearBuilderPawn>(DefaultPawnClass, SelectionPlatform->GetActorLocation(), FRotator::ZeroRotator);
			PlayerController->Possess(BuilderPawn);
		}
	}
}

void AGearGameMode::SpawnNewPlaceables()
{
	PreviewPlaceables.Empty(5);

	for (USceneComponent* Socket : SelectionPlatform->Sockets)
	{
		TSubclassOf<AGearPlaceable>& SpawnClass = AvaliablePlaceables[FMath::RandRange(0, AvaliablePlaceables.Num() - 1)];

		AGearPlaceable* PlaceableActor = GetWorld()->SpawnActor<AGearPlaceable>(SpawnClass, Socket->GetComponentLocation(), Socket->GetComponentRotation());
		PlaceableActor->SetPreview();
		PlaceableActor->AttachPlaceableToComponent(Socket);
		PlaceableActor->Tags.Add(PREVIEW_MODULE_TAG);

		PreviewPlaceables.Add(PlaceableActor);
	}
}

void AGearGameMode::ResetCheckpointsState()
{
	for (ACheckpoint* Checkpoint : GearGameState->CheckpointsStack)
	{
		Checkpoint->LastStartTime = -1;
		Checkpoint->OnRep_LastStartTime();
	}
}

bool AGearGameMode::IsEveryPlayerEliminated() const
{
	for (AGearVehicle* V : GearGameState->Vehicles)
	{
		if (IsValid(V) && !V->IsSpectating())
		{
			return false;
		}
	}

	return true;
}

bool AGearGameMode::ShouldVehicleDie(AGearVehicle* Vehicle, EElimanationReason& EliminationReson)
{
	const bool bOutsideCameraFrustom = GearGameState->VehicleCamera ? GearGameState->VehicleCamera->IsOutsideCameraFrustum(Vehicle) : false;
	if (!Vehicle->IsSpectating() && (Vehicle->IsOutsideTrack() || bOutsideCameraFrustom))
	{
		EliminationReson = bOutsideCameraFrustom ? EElimanationReason::OutsideCameraFrustum : EElimanationReason::Falldown;
		return true;
	}
	return false;
}

void AGearGameMode::DestroyVehicle(AGearVehicle* Vehicle)
{	
	AGearPlayerState* Player = Vehicle->GetPlayerState<AGearPlayerState>();

	GearGameState->Vehicles.Remove(Vehicle);
	Vehicle->Destroy();

	ACheckpoint* NextCheckpoint = GearGameState->GetNextFurthestReachedCheckpoint();

	if (IsValid(NextCheckpoint))
	{
		SpawnVehicleAtCheckpoint(Player, NextCheckpoint, true);
	}
}

void AGearGameMode::DestroyAllVehicles(bool bIncludeSpectators)
{
	for (int i = GearGameState->Vehicles.Num() - 1; i >= 0; i--)
	{
		AGearVehicle* Vehicle = GearGameState->Vehicles[i];
		if (IsValid(Vehicle))
		{
			if (Vehicle->IsSpectating() && !bIncludeSpectators)
			{
				continue;
			}

			DestroyVehicle(Vehicle);
		}
	}
}

void AGearGameMode::RequestSelectingPlaceableForPlayer(AGearPlaceable* Placeable, AGearBuilderPawn* Player)
{
	if (GetGearMatchState() == EGearMatchState::SelectingPlaceables && GearGameState->TimeFromLastTransition() > 2.0f &&  GearGameState->LastGameStateTransitionTime  && IsValid(Placeable) && IsValid(Player) && !Player->HasSelectedPlaceable() && !Placeable->HasOwningPlayer())
	{
		Player->SetSelectedPlaceable(Placeable);
		GearGameState->BroadcastSelectedEvent_Multi(Player->GetPlayerState<AGearPlayerState>(), AGearPlaceable::StaticClass()->GetAuthoritativeClass(), Placeable);
	}
}

void AGearGameMode::RequestPlaceRoadModuleForPlayer(AGearPlayerController* PC, TSubclassOf<AGearRoadModule> RoadModule, bool bMirrorX)
{
	if (GetGearMatchState() == EGearMatchState::Placing && IsValid(PC) && IsValid(RoadModule))
	{
		AGearBuilderPawn* BuilderPawn = PC->GetPawn<AGearBuilderPawn>();

		if (IsValid(BuilderPawn) && IsValid(BuilderPawn->SelectedPlaceableClass) && BuilderPawn->SelectedPlaceableClass->IsChildOf<AGearRoadModule>()
			&& UGearStatics::TraceRoadModule(this, RoadModule, GearGameState->RoadModuleSocketTransform) == ERoadModuleTraceResult::NotColliding)
		{
			if (IsValid(AddRoadModule(RoadModule)))
			{
				BuilderPawn->SelectedPlaceableClass = nullptr;
				BuilderPawn->OnRep_SelectedPlaceableClass();

				GearGameState->BroadcastPlacedEvent_Multi(PC->GetPlayerState<AGearPlayerState>(), RoadModule);
			}

			if (ShouldAddCheckpoint())
			{
				AddCheckpoint();
			}
		}
	}
}

void AGearGameMode::RequestPlaceHazardForPlayer(AGearPlayerController* PC, class UHazardSocketComponent* TargetSocket)
{
	if (GetGearMatchState() == EGearMatchState::Placing && IsValid(PC) && IsValid(TargetSocket) && !TargetSocket->IsOccupied())
	{
		AGearBuilderPawn* BuilderPawn = PC->GetPawn<AGearBuilderPawn>();
		if (IsValid(BuilderPawn) && BuilderPawn->RemainingHazardCount > 0 && IsValid(BuilderPawn->SelectedPlaceableClass) 
			&& BuilderPawn->SelectedPlaceableClass->IsChildOf<AGearHazard>() && TargetSocket->IsCompatibleWithType(BuilderPawn->SelectedPlaceableClass->GetDefaultObject<AGearHazard>()->HazardSocketType))
		{
			TSubclassOf<AGearPlaceable> SpawnClass = BuilderPawn->SelectedPlaceableClass;
			AGearHazard* Hazard = AddHazard(SpawnClass, TargetSocket);
			if (IsValid(Hazard))
			{
				Hazard->SetIdle();

				BuilderPawn->RemainingHazardCount--;
				TargetSocket->MarkOccupied();
				TargetSocket->OnRep_Occupied();

				if (BuilderPawn->RemainingHazardCount <= 0)
				{
					BuilderPawn->SelectedPlaceableClass = nullptr;
					BuilderPawn->OnRep_SelectedPlaceableClass();

					GearGameState->BroadcastPlacedEvent_Multi(PC->GetPlayerState<AGearPlayerState>(), SpawnClass);
				}
			}
		}
	}
}

AGearRoadModule* AGearGameMode::AddRoadModule(TSubclassOf<AGearRoadModule> RoadModule)
{
	FTransform SpawnTransform;

	AActor* SpawnActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), RoadModule, SpawnTransform);
	AGearRoadModule* SpawnRoadModule = Cast<AGearRoadModule>(SpawnActor);

	if (IsValid(SpawnRoadModule))
	{
		SetOwner(GetOwner());
		UGameplayStatics::FinishSpawningActor(SpawnRoadModule, SpawnTransform);

		SpawnRoadModule->MoveToSocketTransform(GearGameState->RoadModuleSocketTransform);
		SpawnRoadModule->SetIdle();
		GearGameState->RoadModuleStack.Add(SpawnRoadModule);
		GearGameState->OnModuleStackChanged();

		GearGameState->WorldMin = FVector::Min(GearGameState->WorldMin, SpawnRoadModule->GetActorLocation());
		GearGameState->WorldMax = FVector::Max(GearGameState->WorldMax, SpawnRoadModule->GetActorLocation());

		return SpawnRoadModule;
	}

	else if (IsValid(SpawnActor))
	{
		SpawnActor->Destroy();
	}

	return nullptr;
}

class AGearHazard* AGearGameMode::AddHazard(TSubclassOf<AGearPlaceable> HazardClass, UHazardSocketComponent* TargetSocket)
{
	FTransform SpawnTransform = TargetSocket->GetComponentTransform();
	AGearHazard* Hazard = GetWorld()->SpawnActor<AGearHazard>(HazardClass, SpawnTransform.GetLocation(), SpawnTransform.Rotator());

	return Hazard;
}

bool AGearGameMode::ShouldAddCheckpoint() const
{
	float DistanceFromLastCheckpoint = 0;
	for (int i = GearGameState->LastPlacedCheckpointModuleStackIndex + 1; i < GearGameState->RoadModuleStack.Num(); i++)
	{
		check(IsValid(GearGameState->RoadModuleStack[i]));
		DistanceFromLastCheckpoint += GearGameState->RoadModuleStack[i]->RoadLength;	
	}

	return DistanceFromLastCheckpoint > CheckpointDistance;
}

void AGearGameMode::AddCheckpoint()
{
	check(IsValid(CheckpointModuleClass) && IsValid(CheckpointClass));

	AGearRoadModule* Module = AddRoadModule(CheckpointModuleClass);
	if (IsValid(Module))
	{
		FTransform SocketTransform = Module->RoadMesh->GetSocketTransform(TEXT("Checkpoint"));
		ACheckpoint* Checkpoint = GetWorld()->SpawnActor<ACheckpoint>(CheckpointClass, SocketTransform.GetLocation(), SocketTransform.Rotator());
		Checkpoint->CheckpointIndex = GearGameState->CheckpointsStack.Num();
		
		check(Checkpoint);
		GearGameState->CheckpointsStack.Add(Checkpoint);
		GearGameState->LastPlacedCheckpointModuleStackIndex = GearGameState->RoadModuleStack.Num() - 1;
	}
}

void AGearGameMode::HandleMatchAborted()
{
	Super::HandleMatchAborted();

	UE_LOG(LogTemp, Error, TEXT("Match Aborted!"));
}

void AGearGameMode::SetGearMatchState(EGearMatchState InGearMatchState)
{
	EGearMatchState OldState = GetGearMatchState();

	GearGameState->GearMatchState = InGearMatchState;
	GearGameState->LastGameStateTransitionTime = GearGameState->GetServerWorldTimeSeconds();
	GearGameState->OnRep_GearMatchState(OldState);
}

EGearMatchState AGearGameMode::GetGearMatchState() const
{
	return GearGameState->GearMatchState;
}

void AGearGameMode::AssignPlaceablesToUnowningPlayers()
{
	TArray<AGearBuilderPawn*> UnowningPlayers;
	TArray<AGearPlaceable*> UnownedPlaceables;

	for (APlayerState* Player : GameState->PlayerArray)
	{
		AGearBuilderPawn* BuilderPawn = Cast<AGearBuilderPawn>(Player->GetPawn());
		if (IsValid(BuilderPawn) && !BuilderPawn->HasSelectedPlaceable())
		{
			UnowningPlayers.Add(BuilderPawn);
		}
	}

	for (AGearPlaceable* Placeable : PreviewPlaceables)
	{
		if (!Placeable->HasOwningPlayer())
		{
			UnownedPlaceables.Add(Placeable);
		}
	}

	for (int i = 0; i < UnowningPlayers.Num(); i++)
	{
		check(i < UnownedPlaceables.Num());

		UnowningPlayers[i]->SetSelectedPlaceable(UnownedPlaceables[i]);
	}
}

void AGearGameMode::StartSelectingPlaceables()
{
	SpawnSelectionPlatform();
	SpawnNewBuilderPawns();
	SpawnNewPlaceables();

	GetWorld()->GetTimerManager().SetTimer(SelectingPiecesTimerHandle, FTimerDelegate::CreateUObject(this, &AGearGameMode::StartPlaceing, false), UGameVariablesBFL::GV_PieceSelectionTimeLimit(), false);

	UE_LOG(LogTemp, Warning, TEXT("Selecting pieces"));
	SetGearMatchState(EGearMatchState::SelectingPlaceables);
}

bool AGearGameMode::IsEveryPlayerSelectedPlaceables()
{
	for (APlayerState* Player : GameState->PlayerArray)
	{
		AGearBuilderPawn* BuilderPawn = Cast<AGearBuilderPawn>(Player->GetPawn());
		if (IsValid(BuilderPawn) && !BuilderPawn->HasSelectedPlaceable())
		{
			return false;
		}
	}

	return true;
}

void AGearGameMode::StartPlaceing(bool bEveryPlayerIsReady)
{
	GetWorld()->GetTimerManager().ClearTimer(SelectingPiecesTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(PlacingTimerHandle, FTimerDelegate::CreateUObject(this, &AGearGameMode::EndPlaceing), UGameVariablesBFL::GV_PlacingTimeLimit(), false);

	if (!bEveryPlayerIsReady)
	{
		AssignPlaceablesToUnowningPlayers();
	}

	for (APlayerState* PlayerState : GearGameState->PlayerArray)
	{
		AGearBuilderPawn* BuilderPawn = Cast<AGearBuilderPawn>(PlayerState->GetPawn());
		if (IsValid(BuilderPawn) && BuilderPawn->HasSelectedPlaceable())
		{
			BuilderPawn->SelectedPlaceableClass = BuilderPawn->SelectedPlaceable->GetClass();
			BuilderPawn->SelectedPlaceable = nullptr;

			if (BuilderPawn->SelectedPlaceableClass->IsChildOf(AGearHazard::StaticClass()))
			{
				BuilderPawn->RemainingHazardCount = BuilderPawn->SelectedPlaceableClass->GetDefaultObject<AGearHazard>()->PlacingCount;
			}

			BuilderPawn->OnRep_SelectedPlaceableClass();
		}
	}

	for (AGearPlaceable* PreviewPlaceable : PreviewPlaceables)
	{
		PreviewPlaceable->Destroy();
	}

	SelectionPlatform->Destroy();

	UE_LOG(LogTemp, Warning, TEXT("start placing pieces"));
	SetGearMatchState(EGearMatchState::Placing);
}

void AGearGameMode::EndPlaceing()
{
	if (GearGameState->CheckpointsStack.Num() > 1)
	{
		StartRacing();
	}

	else
	{
		DestroyPawns();
		StartSelectingPlaceables();
	}
}

bool AGearGameMode::IsEveryPlayerPlaced()
{
	for (APlayerState* Player : GameState->PlayerArray)
	{
		AGearBuilderPawn* BuilderPawn = Cast<AGearBuilderPawn>(Player->GetPawn());
		if (IsValid(BuilderPawn) && IsValid(BuilderPawn->SelectedPlaceableClass))
		{
			return false;
		}
	}

	return true;
}

void AGearGameMode::DestroyActors()
{
	TArray<AActor*> SpawnedActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT(SPAWNED_ACTOR_TAG), SpawnedActors);

	for (AActor* Actor : SpawnedActors)
	{
		Actor->Destroy();
	}
}

void AGearGameMode::SpawnActors()
{
	for (AGearRoadModule* RoadModule : GearGameState->RoadModuleStack)
	{
		check(IsValid(RoadModule));

		TInlineComponentArray<USpawnableSocket*> Sockets(RoadModule);
		for (USpawnableSocket* SpawnableSocket : Sockets)
		{
			check(IsValid(SpawnableSocket) && IsValid(SpawnableSocket->SpawnClass));

			FTransform SpawnTransform = SpawnableSocket->GetComponentTransform();
			FActorSpawnParameters SpawnParameter;
			SpawnParameter.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AActor* Actor = GetWorld()->SpawnActor<AActor>(SpawnableSocket->SpawnClass->GetAuthoritativeClass(),
				SpawnTransform.GetLocation(), SpawnTransform.Rotator(), SpawnParameter);

			Actor->Tags.Add(TEXT(SPAWNED_ACTOR_TAG));
		}
	}
}

void AGearGameMode::DestroyPawns()
{
	for (APlayerState* Player : GameState->PlayerArray)
	{
		if (IsValid(Player->GetPawn()))
		{
			Player->GetPawn()->Destroy();
		}
	}
}

void AGearGameMode::SpawnVehicleAtCheckpoint(AGearPlayerState* Player, ACheckpoint* Checkpoint, bool GrantInvincibility)
{
	UVehicleStart* VehicleStart = IsValid(Checkpoint) ? Checkpoint->GetVehicleStart() : nullptr;
	if (IsValid(VehicleStart))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
 		FVector SpawnLocation = VehicleStart->GetComponentLocation();
 		FRotator SpawnRotation = VehicleStart->GetComponentRotation();

		AGearVehicle* GearVehicle = GetWorld()->SpawnActor<AGearVehicle>(Player->VehicleClass->GetAuthoritativeClass(), SpawnLocation, SpawnRotation, SpawnParams);
		Player->GetPlayerController()->Possess(GearVehicle);

		if (GrantInvincibility)
		{
			GearVehicle->GrantInvincibility();
			GearVehicle->OnRep_GrantedInvincibility();
		}

		GearGameState->RegisterVehicleAtCheckpoint(GearVehicle, Checkpoint);

		UE_LOG(LogTemp, Warning, TEXT("spawned %s %s"), *GetName(), GrantInvincibility ? TEXT("with invincibility") : TEXT(""));
	}
}

void AGearGameMode::StartRacingAtCheckpoint(ACheckpoint* Checkpoint, AGearVehicle* InstgatorVehicle)
{
	check(IsValid(Checkpoint));

	GetWorld()->GetTimerManager().ClearTimer(RacingTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(RacingEndDelayTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(RacingTimerHandle, FTimerDelegate::CreateUObject(this, &AGearGameMode::DestroyAllVehicles, false)
		, UGameVariablesBFL::GV_RacingTimeLimit(), false);
	
	const bool bNeedsCountDown = !IsValid(InstgatorVehicle);

	GearGameState->UpdateFurthestReachedCheckpoint(Checkpoint->CheckpointIndex);

	// if there was no vehicle reached to checkpoint start with countdown
	if (bNeedsCountDown)
	{
		GearGameState->LastCountDownTime = GetWorld()->GetTimeSeconds();
		Checkpoint->LastStartTime = GetWorld()->GetTimeSeconds();
		Checkpoint->OnRep_LastStartTime();

		UE_LOG(LogTemp, Warning, TEXT("start count down"));
	}

	bool bEveryPlayerEliminated = true;

	for (APlayerState* PlayerState : GearGameState->PlayerArray)
	{
		AGearPlayerState* GearPlayerState = Cast<AGearPlayerState>(PlayerState);
		AGearVehicle* GearVehicle = Cast<AGearVehicle>(GearPlayerState->GetPawn());

		if (IsValid(GearVehicle))
		{
			GearVehicle->UpdateStateToVehicle(InstgatorVehicle);

			bEveryPlayerEliminated = false;
		}

		else
		{
			SpawnVehicleAtCheckpoint(GearPlayerState, Checkpoint, false);
		}
	}

	if (bEveryPlayerEliminated && IsValid(GearGameState->VehicleCamera))
	{
		GearGameState->VehicleCamera->UpdateCamera();
		GearGameState->VehicleCamera->MarkTeleport();
		GearGameState->VehicleCamera->UpdateCameraMatrix();
	}

	GearGameState->LastCheckpointStartTime = GetWorld()->GetTimeSeconds();

	GearGameState->BroadcastRaceStartEvent_Multi(GearGameState->GetServerWorldTimeSeconds(), bNeedsCountDown);
}

void AGearGameMode::StartRacing()
{
	GetWorld()->GetTimerManager().ClearTimer(PlacingTimerHandle);

	SpawnActors();

	DestroyPawns();
	GearGameState->Vehicles.Empty(4);
	GearGameState->FurthestReachedDistace = 0;
	GearGameState->UpdateFurthestReachedCheckpoint(0);
	GearGameState->ClearCheckpointResults();
	GearGameState->ClearOccupiedVehicleStarts();

	StartRacingAtCheckpoint(GearGameState->GetCheckPointAtIndex(0), nullptr);

	SetGearMatchState(EGearMatchState::Racing);
	UE_LOG(LogTemp, Warning, TEXT("start racing"));
}

void AGearGameMode::StartScoreboard()
{
	GetWorld()->GetTimerManager().ClearTimer(RacingTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(RacingEndDelayTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(ScoreboardTimerHandle,
		FTimerDelegate::CreateUObject(this, &AGearGameMode::ScoreboardLifespanFinished), GearGameState->GetEstimatedScoreboardLifespan(), false);

	DestroyActors();

	ResetCheckpointsState();

	UE_LOG(LogTemp, Warning, TEXT("start scoreboard"));
	SetGearMatchState(EGearMatchState::Scoreboard);
}

void AGearGameMode::ScoreboardLifespanFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(ScoreboardTimerHandle);

	if (GearGameState->IsAnyPlayerWinning())
	{
		GameFinished();
	}
	else
	{
		StartNewRound();
	}
}

void AGearGameMode::StartNewRound()
{
	UE_LOG(LogTemp, Warning, TEXT("starting new round"));

	GearGameState->RoundNumber++;

	StartSelectingPlaceables();
}

void AGearGameMode::GameFinished()
{
	

	UE_LOG(LogTemp, Warning, TEXT("game finished"));
	SetGearMatchState(EGearMatchState::GameFinished);
}

void AGearGameMode::VehicleReachedCheckpoint(AGearVehicle* Vehicle, ACheckpoint* TargetCheckpoint)
{
	check(IsValid(Vehicle) && IsValid(TargetCheckpoint));

	if (Vehicle->TargetCheckpoint == TargetCheckpoint->CheckpointIndex)
	{
		AGearPlayerState* GearPlayerState = Vehicle->GetPlayerState<AGearPlayerState>();
		check(GearPlayerState);

		const int CheckpointIndex = TargetCheckpoint->CheckpointIndex;

		GearGameState->CheckpointResults[CheckpointIndex - 1].Add(GearPlayerState);
		Vehicle->TargetCheckpoint = CheckpointIndex + 1;
		UE_LOG(LogTemp, Warning, TEXT("%s is %i player to reached checkpoint number %i"), *GearPlayerState->GetPlayerName(), GearGameState->CheckpointResults[CheckpointIndex - 1].PlayerList.Num(), CheckpointIndex);

		AGearPlayerState* Player = Vehicle->GetPlayerState<AGearPlayerState>();
		int32 Position = GearGameState->CheckpointResults[CheckpointIndex - 1].PlayerList.Num();

		GearGameState->BroadcastReachedCheckpointEvent_Multi(Player, TargetCheckpoint, Position, GearGameState->CheckpointsStack.Num(), GearGameState->GetServerWorldTimeSeconds());
		TargetCheckpoint->PlayerReachedCheckpoint_Multi(Player, Position);

		if (CheckpointIndex > GearGameState->FurthestReachedCheckpoint)
		{
			GearGameState->UpdateFurthestReachedCheckpoint(CheckpointIndex);

			if (GearGameState->CheckpointsStack.Top() != TargetCheckpoint)
			{
				StartRacingAtCheckpoint(TargetCheckpoint, Vehicle);
			}
		}

		if (GearGameState->CheckpointsStack.Top() == TargetCheckpoint)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s finished race"), *GearPlayerState->GetName());
			DestroyVehicle(Vehicle);
		}
	}
}

TSubclassOf<class AGearAbility> AGearGameMode::GetRandomAbility()
{
	if (AvaliableAbilities.Num() > 0)
	{
		return AvaliableAbilities[FMath::RandRange(0, AvaliableAbilities.Num() - 1)];
	}

	return nullptr;
}

bool AGearGameMode::CheckIsEveryPlayerReady()
{
	bool EveryClientIsReady = true;
	
	// until all clients doesn't respond to server client don`t start game
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AGearPlayerController* GearController = Cast<AGearPlayerController>(*Iterator);	
		if (IsValid(GearController) && !GearController->IsReady)
		{
			EveryClientIsReady = false;
			GearController->PeekClientIsReady();
		}
	}
	
	return NumTravellingPlayers == 0 && NumPlayers > 1 && EveryClientIsReady;
}

void AGearGameMode::AllPlayerJoined()
{	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGearGameMode::StartNewRound, UGameVariablesBFL::GV_AllPlayerJoinToGameStartDelay());

	GearGameState->bEveryPlayerReady = true;
	GearGameState->OnRep_EveryPlayerReady();

	UE_LOG(LogTemp, Warning, TEXT("all player joined"));
}