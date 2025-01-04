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
#include "Placeable/PlaceableSocket.h"
#include "Placeable/PlaceableSpawnPoint.h"
#include "Placeable/SpawnableSocket.h"

#include "Utils/GameVariablesBFL.h"
#include "kismet/GameplayStatics.h"

#define SPAWNED_ACTOR_TAG "Spawned"

AGearGameMode::AGearGameMode()
{
	PlayerControllerClass = AGearPlayerController::StaticClass();
	GameStateClass = AGearGameState::StaticClass();
	PlayerStateClass = AGearPlayerState::StaticClass();

	bUseSeamlessTravel = true;

	GearMatchState = EGearMatchState::WaitingForPlayerToJoin;
	CheckpointDistance = 1000.0f;
}

void AGearGameMode::BeginPlay()
{
	Super::BeginPlay();

	GearGameState = GetGameState<AGearGameState>();
	check(IsValid(GearGameState));


	bool bFailed = false;

	if (!LoadPlaceables())
	{
		UE_LOG(LogGameMode, Error, TEXT("Not enough hazards found"));
		bFailed = true;
	}

	if (!LoadPlaceableSpawnPoints())
	{
		UE_LOG(LogGameMode, Error, TEXT("Not enough hazards preview spawn point found"));
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

	if (GearMatchState == EGearMatchState::WaitingForPlayerToJoin)
	{
		if (CheckIsEveryPlayerReady())
		{
			AllPlayerJoined();
		}
	}

	else if (GearMatchState == EGearMatchState::SelectingPlaceables)
	{
		if (IsEveryPlayerSelectedPlaceables())
		{
			StartPlaceing(true);
		}
	}

	else if (GearMatchState == EGearMatchState::Placing)
	{
		if (IsEveryPlayerPlaced())
		{
			EndPlaceing();
			//StartRacing(true);
		}
	}

	else if (GearMatchState == EGearMatchState::Racing)
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
				GearGameState->BroadcastEliminationEvent_Multi(Vehicle->GetPlayerState<AGearPlayerState>(), ElimanationReason);
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

	if (bEveryPlayerEliminated)
	{
		for (int i = GearGameState->Vehicles.Num() - 1; i >= 0; i--)
		{
			AGearVehicle* Vehicle = GearGameState->Vehicles[i];
			Vehicle->Destroy();
		}
		GearGameState->Vehicles.Empty(4);

		ACheckpoint* NextCheckpoint = GearGameState->GetNextFurthestReachedCheckpoint();

		if (IsValid(NextCheckpoint))
		{
			GearGameState->ClearOccupiedVehicleStarts();
			StartRacingAtCheckpoint(NextCheckpoint, nullptr);
		}

		else
		{
			StartScoreboard();
		}
	}
}

bool AGearGameMode::ShouldAbort()
{
	return NumTravellingPlayers == 0 && NumPlayers < 2;
}

void AGearGameMode::SpawnNewBuilderPawns()
{
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AGearPlayerController* PlayerController = Cast<AGearPlayerController>(*It);
		if (IsValid(PlayerController))
		{
			AGearBuilderPawn* BuilderPawn = GetWorld()->SpawnActor<AGearBuilderPawn>(DefaultPawnClass);
			PlayerController->Possess(BuilderPawn);
		}
	}
}

void AGearGameMode::SpawnNewPlaceables()
{
	PreviewPlaceables.Empty(5);

	for (APlaceableSpawnPoint* SpawnPoint : HazardPreviewSpawnPoints)
	{
		// TODO: implement hazard spawning logic, for now spawn randomly
		TSubclassOf<AGearPlaceable>& SpawnClass = AvaliablePlaceables[ FMath::RandRange(0, AvaliablePlaceables.Num()-1) ].Class;

		AGearPlaceable* PlaceableActor = GetWorld()->SpawnActor<AGearPlaceable>(SpawnClass, SpawnPoint->GetTransform());
		PlaceableActor->SetPreview();
		PlaceableActor->AttachToSpawnPoint(SpawnPoint);

		PreviewPlaceables.Add(PlaceableActor);
	}
}

bool AGearGameMode::LoadPlaceables()
{
	AvaliablePlaceables.Empty();

	if (IsValid(PlaceableSpawnRulesDataTable))
	{
		TArray<FPlaceableDescription*> RawHazards;
		PlaceableSpawnRulesDataTable->GetAllRows<FPlaceableDescription>("Reading hazard rules", RawHazards);

		for (FPlaceableDescription* Desc : RawHazards)
		{
			if (Desc->bDepracated)
			{
				continue;
			}

			TSubclassOf<AGearPlaceable> HazardClass = StaticLoadClass(AGearPlaceable::StaticClass(), nullptr, *Desc->ClassPath);
			if (IsValid(HazardClass))
			{
				FPlaceableDescription NewDesc = *Desc;
				NewDesc.Class = HazardClass;
				
				AvaliablePlaceables.Add(NewDesc);
			}
		}

		if (AvaliablePlaceables.Num() == 0)
		{
			return false;
		}

		return true;
	}

	return false;
}

bool AGearGameMode::LoadPlaceableSpawnPoints()
{
	HazardPreviewSpawnPoints.Empty(5);
	
	TArray<AActor*> SpawnActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlaceableSpawnPoint::StaticClass(), SpawnActors);

	for (AActor* Actor : SpawnActors)
	{
		APlaceableSpawnPoint* SpawnPoint = Cast<APlaceableSpawnPoint>(Actor);
		if (IsValid(SpawnPoint))
		{
			HazardPreviewSpawnPoints.Add(SpawnPoint);
		}
	}

	return HazardPreviewSpawnPoints.Num() == 5;
}

bool AGearGameMode::IsEveryPlayerEliminated() const
{
	bool bEveryPlayerEliminated = true;
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

void AGearGameMode::RequestSelectingPlaceableForPlayer(AGearPlaceable* Placeable, AGearBuilderPawn* Player)
{
	if (GearMatchState == EGearMatchState::SelectingPlaceables && IsValid(Placeable) && IsValid(Player) && !Player->HasSelectedPlaceable() && !Placeable->HasOwningPlayer())
	{
		Player->SetSelectedPlaceable(Placeable);
		FDetachmentTransformRules DetachmentRule(EDetachmentRule::KeepWorld, false);
		Placeable->DetachFromActor(DetachmentRule);
		GearGameState->BroadcastSelectedEvent_Multi(Player->GetPlayerState<AGearPlayerState>(), AGearPlaceable::StaticClass()->GetAuthoritativeClass());
	}
}

void AGearGameMode::RequestPlaceRoadModuleForPlayer(AGearPlayerController* PC, TSubclassOf<AGearRoadModule> RoadModule, bool bMirrorX)
{
	if (GearMatchState == EGearMatchState::Placing && IsValid(RoadModule))
	{
		AGearBuilderPawn* BuilderPawn = PC->GetPawn<AGearBuilderPawn>();
		if (IsValid(BuilderPawn) && !BuilderPawn->bPlacedModule && UGearStatics::TraceRoadModule(this, RoadModule, GearGameState->RoadModuleSocketTransform) == ERoadModuleTraceResult::NotColliding)
		{
			if (IsValid(AddRoadModule(RoadModule)))
			{
				BuilderPawn->SelectedPlaceableClass = nullptr;
				BuilderPawn->OnRep_SelectedPlaceableClass();

				BuilderPawn->bPlacedModule = true;
				GearGameState->BroadcastPlacedEvent_Multi(PC->GetPlayerState<AGearPlayerState>(), RoadModule);
			}

			if (ShouldAddCheckpoint())
			{
				AddCheckpoint();
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

		return SpawnRoadModule;
	}

	else if (IsValid(SpawnActor))
	{
		SpawnActor->Destroy();
	}

	return nullptr;
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
	GearGameState->GearMatchState = InGearMatchState;
	GearGameState->LastGameStateTransitionTime = GearGameState->GetServerWorldTimeSeconds();
	GearGameState->OnRep_GearMatchState(GearMatchState);

	GearMatchState = InGearMatchState;
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
			BuilderPawn->OnRep_SelectedPlaceableClass();
		}
	}

	for (AGearPlaceable* PreviewPlaceable : PreviewPlaceables)
	{
		PreviewPlaceable->Destroy();
	}

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
	
	const bool bNeedsCountDown = !IsValid(InstgatorVehicle);

	GearGameState->FurthestReachedCheckpoint = Checkpoint->CheckpointIndex;
	GearGameState->OnRep_FurthestReachedCheckpoint();

	// if there was no vehicle reached to checkpoint start with countdown
	if (bNeedsCountDown)
	{
		GearGameState->LastCountDownTime = GetWorld()->GetTimeSeconds();
		Checkpoint->StartCountDown(GetWorld()->GetTimeSeconds());

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
}

void AGearGameMode::StartRacing()
{
	GetWorld()->GetTimerManager().ClearTimer(PlacingTimerHandle);

	SpawnActors();

	DestroyPawns();
	GearGameState->Vehicles.Empty(4);
	GearGameState->FurthestReachedDistace = 0;
	GearGameState->FurthestReachedCheckpoint = 0;
	GearGameState->OnRep_FurthestReachedCheckpoint();
	GearGameState->ClearCheckpointResults();
	GearGameState->ClearOccupiedVehicleStarts();

	StartRacingAtCheckpoint(GearGameState->GetCheckPointAtIndex(0), nullptr);

	SetGearMatchState(EGearMatchState::Racing);
	UE_LOG(LogTemp, Warning, TEXT("start racing"));
}

void AGearGameMode::StartScoreboard()
{
	GetWorld()->GetTimerManager().SetTimer(ScoreboardTimerHandle,
		FTimerDelegate::CreateUObject(this, &AGearGameMode::ScoreboardLifespanFinished), GearGameState->GetEstimatedScoreboardLifespan(), false);

	DestroyActors();

	UE_LOG(LogTemp, Warning, TEXT("start scoreboard"));
	SetGearMatchState(EGearMatchState::Scoreboard);
}

void AGearGameMode::ScoreboardLifespanFinished()
{
	ScoreboardTimerHandle.Invalidate();

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

		GearGameState->BroadcastReachedCheckpointEvent_Multi(Player, TargetCheckpoint, Position);
		TargetCheckpoint->PlayerReachedCheckpoint_Multi(Player, Position);

		if (CheckpointIndex > GearGameState->FurthestReachedCheckpoint)
		{
			GearGameState->FurthestReachedCheckpoint = CheckpointIndex;
			GearGameState->OnRep_FurthestReachedCheckpoint();
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

	UE_LOG(LogTemp, Warning, TEXT("all player joined"));
	SetGearMatchState(EGearMatchState::AllPlayersJoined);
}