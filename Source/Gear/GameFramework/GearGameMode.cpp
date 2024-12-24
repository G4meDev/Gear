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

	if (GearMatchState == EGearMatchState::SelectingPlaceables)
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
			StartRacing(true);
		}
	}

	else if (GearMatchState == EGearMatchState::Racing)
	{
		if (GearGameState->Vehicles.Num() == 0)
		{
			if (GearGameState->FurthestReachedCheckpoint < GearGameState->CheckpointsStack.Num() - 2)
			{
				StartRacingAtCheckpoint(GearGameState->FurthestReachedCheckpoint + 1, true);
			}

			else
			{
				StartScoreboard();
			}
		}
	}

	if (ShouldAbort())
	{
		AbortMatch();
	}
}


void AGearGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	SetGearMatchState(EGearMatchState::GameFinished);
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

void AGearGameMode::RequestSelectingPlaceableForPlayer(AGearPlaceable* Placeable, AGearBuilderPawn* Player)
{
	if (GearMatchState == EGearMatchState::SelectingPlaceables && IsValid(Placeable) && IsValid(Player) && !Placeable->HasOwningPlayer())
	{
		Player->SetSelectedPlaceable(Placeable);
	}
}

void AGearGameMode::RequestPlaceRoadModuleForPlayer(AGearPlayerController* PC, TSubclassOf<AGearRoadModule> RoadModule, UPlaceableSocket* TargetSocket, bool bMirrorX)
{
	if (GearMatchState == EGearMatchState::Placing && IsValid(RoadModule) && IsValid(TargetSocket) && GearGameState->GetRoadStackAttachableSocket() == TargetSocket && !TargetSocket->IsOccupied())
	{
		// TODO: sweep test

		AGearBuilderPawn* BuilderPawn = PC->GetPawn<AGearBuilderPawn>();

		if (IsValid(BuilderPawn))
		{
			if (IsValid(AddRoadModule(RoadModule, bMirrorX)))
			{
				BuilderPawn->SelectedPlaceableClass = nullptr;
				BuilderPawn->OnRep_SelectedPlaceableClass();				
			}

			if (ShouldAddCheckpoint())
			{
				AddCheckpoint();
			}
		}
	}
}



AGearRoadModule* AGearGameMode::AddRoadModule(TSubclassOf<AGearRoadModule> RoadModule, bool bMirrorX)
{
	FTransform SpawnTransform;

	AActor* SpawnActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), RoadModule, SpawnTransform);
	AGearRoadModule* SpawnRoadModule = Cast<AGearRoadModule>(SpawnActor);

	if (IsValid(SpawnRoadModule))
	{
		SpawnRoadModule->bMirrorX = bMirrorX;
		SpawnRoadModule->bShouldNotifyGameState = true;
		SpawnRoadModule->bEnabledSelectionBox = false;
		SpawnRoadModule->OnRep_EnabledSelectionBox();
		SetOwner(GetOwner());
		UGameplayStatics::FinishSpawningActor(SpawnRoadModule, SpawnTransform);

		SpawnRoadModule->MoveToSocket(GearGameState->GetRoadStackAttachableSocket(), bMirrorX);
		GearGameState->RoadModuleStack.Add(SpawnRoadModule);
		GearGameState->OnRep_RoadModuleStack();

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

	AGearRoadModule* Module = AddRoadModule(CheckpointModuleClass, false);
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

bool AGearGameMode::ReadyToEndMatch_Implementation()
{
	return false;
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
	GetWorld()->GetTimerManager().SetTimer(PlacingTimerHandle, FTimerDelegate::CreateUObject(this, &AGearGameMode::StartRacing, false), UGameVariablesBFL::GV_PlacingTimeLimit(), false);

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

void AGearGameMode::PlaceUnplaced()
{
	TArray<AGearBuilderPawn*> UnplacedRoadModulePawns;

	for (APlayerState* Player : GameState->PlayerArray)
	{
		AGearBuilderPawn* BuilderPawn = Cast<AGearBuilderPawn>(Player->GetPawn());
		if (IsValid(BuilderPawn) && BuilderPawn->SelectedPlaceableClass && BuilderPawn->SelectedPlaceableClass->IsChildOf(AGearRoadModule::StaticClass()))
		{
			UnplacedRoadModulePawns.Add(BuilderPawn);
		}
	}

	for (AGearBuilderPawn* BuilderPawn : UnplacedRoadModulePawns)
	{
		AGearPlayerController* GearPlayerController = BuilderPawn->GetController<AGearPlayerController>();
		TSubclassOf<AGearRoadModule> RoadModuleClass = BuilderPawn->SelectedPlaceableClass->GetAuthoritativeClass();

		if (IsValid(GearPlayerController))
		{
			RequestPlaceRoadModuleForPlayer(GearPlayerController, RoadModuleClass, GearGameState->GetRoadStackAttachableSocket(), false);
		}
	}
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

void AGearGameMode::StartRacingAtCheckpoint(int CheckpointIndex, bool bWithCountDown)
{
	ACheckpoint* Checkpoint = GearGameState->GetCheckPointAtIndex(CheckpointIndex);
	check(IsValid(Checkpoint));

	GearGameState->FurthestReachedCheckpoint = CheckpointIndex;

	bool bEveryPlayerEliminated = true;

	int i = 0;
	for (APlayerState* PlayerState : GearGameState->PlayerArray)
	{
		AGearPlayerState* GearPlayerState = Cast<AGearPlayerState>(PlayerState);

		if (!IsValid(GearPlayerState->GetPawn()))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FVector SpawnLocation = Checkpoint->StartPoints[i]->GetComponentLocation();
			FRotator SpawnRotation = Checkpoint->StartPoints[i]->GetComponentRotation();

			AGearVehicle* GearVehicle = GetWorld()->SpawnActor<AGearVehicle>(GearPlayerState->VehicleClass->GetAuthoritativeClass(), SpawnLocation, SpawnRotation, SpawnParams);
			GearPlayerState->GetPlayerController()->Possess(GearVehicle);

			GearGameState->RegisterVehicleAtCheckpoint(GearVehicle, CheckpointIndex);

			i++;
		}

		else
		{
			bEveryPlayerEliminated = false;
		}
	}

	if (bEveryPlayerEliminated && IsValid(GearGameState->VehicleCamera))
	{
		GearGameState->VehicleCamera->UpdateCamera();
		GearGameState->VehicleCamera->MarkTeleport();
		GearGameState->VehicleCamera->UpdateCameraMatrix();
	}

	if (bWithCountDown)
	{
		GearGameState->CheckpointsStack[GearGameState->FurthestReachedCheckpoint]->StartRace(GetWorld()->GetTimeSeconds());
	}
}

void AGearGameMode::StartRacing(bool bEveryPlayerPlaced)
{
	GetWorld()->GetTimerManager().ClearTimer(PlacingTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(RacingWaitTimerHandle, FTimerDelegate::CreateUObject(this, &AGearGameMode::RacingWaitTimeFinished), UGameVariablesBFL::GV_CountDownDuration(), false);

	if (!bEveryPlayerPlaced)
	{
		PlaceUnplaced();
	}

	SpawnActors();

	DestroyPawns();
	GearGameState->Vehicles.Empty(4);
	GearGameState->FurthestReachedDistace = 0;
	GearGameState->FurthestReachedCheckpoint = 0;
	GearGameState->ClearCheckpointResults();

	StartRacingAtCheckpoint(0, true);

	UE_LOG(LogTemp, Warning, TEXT("start racing"));
	SetGearMatchState(EGearMatchState::Racing_WaitTime);
}

void AGearGameMode::RacingWaitTimeFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(RacingWaitTimerHandle);



	SetGearMatchState(EGearMatchState::Racing);
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

		if (CheckpointIndex > GearGameState->FurthestReachedCheckpoint)
		{
			GearGameState->FurthestReachedCheckpoint = CheckpointIndex;
			if (GearGameState->CheckpointsStack.Top() != TargetCheckpoint)
			{
				StartRacingAtCheckpoint(TargetCheckpoint->CheckpointIndex, false);
			}
		}

		if (GearGameState->CheckpointsStack.Top() == TargetCheckpoint)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s finished race"), *GearPlayerState->GetName());
			Vehicle->Destroy();
		}
	}
}

bool AGearGameMode::ReadyToStartMatch_Implementation()
{
	return CheckIsEveryPlayerReady();
}

void AGearGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchIsWaitingToStart();

	UE_LOG(LogTemp, Warning, TEXT("match started."));

	AllPlayerJoined();
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