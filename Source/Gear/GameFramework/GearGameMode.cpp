// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearGameMode.h"
#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearGameState.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearBuilderPawn.h"
#include "Vehicle/GearVehicle.h"

#include "Placeable/GearPlaceable.h"
#include "Placeable/GearRoadModule.h"
#include "Placeable/PlaceableSocket.h"
#include "Placeable/PlaceableSpawnPoint.h"

#include "Utils/GameVariablesBFL.h"
#include "kismet/GameplayStatics.h"

AGearGameMode::AGearGameMode()
{
	PlayerControllerClass = AGearPlayerController::StaticClass();
	GameStateClass = AGearGameState::StaticClass();
	PlayerStateClass = AGearPlayerState::StaticClass();

	bUseSeamlessTravel = true;

	GearMatchState = EGearMatchState::WaitingForPlayerToJoin;
}

void AGearGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!LoadPlaceables())
	{
		UE_LOG(LogGameMode, Error, TEXT("Not enough hazards found"));
		AbortMatch();
	}

	if (!LoadPlaceableSpawnPoints())
	{
		UE_LOG(LogGameMode, Error, TEXT("Not enough hazards preview spawn point found"));
		AbortMatch();
	}

	AGearGameState* GearGameState = GetGameState<AGearGameState>();
	if (!GearGameState || !GearGameState->FindStartRoadModuleAndAddToStack())
	{
		UE_LOG(LogGameMode, Error, TEXT("finding first raod module failed"));
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

	if (ShouldAbort())
	{
		AbortMatch();
	}
}


void AGearGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	SetGearMatchState(EGearMatchState::Ended);
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
	if (GearMatchState == EGearMatchState::Placing && IsValid(RoadModule) && IsValid(TargetSocket) && !TargetSocket->IsOccupied())
	{
		// TODO: sweep test

		AGearGameState* GearGameState = GetGameState<AGearGameState>();
		AGearBuilderPawn* BuilderPawn = PC->GetPawn<AGearBuilderPawn>();

		if (IsValid(GearGameState) && IsValid(BuilderPawn))
		{
			FTransform SpawnTransform;

			AActor* SpawnActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), RoadModule, SpawnTransform);
			AGearRoadModule* SpawnRoadModule = Cast<AGearRoadModule>(SpawnActor);
			if (IsValid(SpawnRoadModule))
			{
				SpawnRoadModule->bMirrorX = bMirrorX;
				SpawnRoadModule->bShouldNotifyGameState = true;
				SetOwner(GetOwner());
				UGameplayStatics::FinishSpawningActor(SpawnRoadModule, SpawnTransform);

				SpawnRoadModule->MoveToSocket(TargetSocket, bMirrorX);
				GearGameState->RoadModuleStack.Add(SpawnRoadModule);
				GearGameState->OnRep_RoadModuleStack();

				BuilderPawn->SelectedPlaceableClass = nullptr;
				BuilderPawn->OnRep_SelectedPlaceableClass();
			}
			else
			{
				SpawnActor->Destroy();
			}
		}
	}
}

void AGearGameMode::HandleMatchAborted()
{
	Super::HandleMatchAborted();

	UE_LOG(LogTemp, Error, TEXT("Match Aborted!"));
}

void AGearGameMode::SetGearMatchState(EGearMatchState InGearMatchState)
{
	AGearGameState* GearGameState = GetGameState<AGearGameState>();
	if (IsValid(GearGameState))
	{
		GearGameState->GearMatchState = InGearMatchState;
		GearGameState->LastGameStateTransitionTime = GearGameState->GetServerWorldTimeSeconds();
		GearGameState->OnRep_GearMatchState(GearMatchState);
	}

	GearMatchState = InGearMatchState;
}

void AGearGameMode::StartFirstPhase()
{	
	StartSelectingPlaceables();
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

	AGearGameState* GearGameState = GetGameState<AGearGameState>();
	if (IsValid(GearGameState))
	{
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
		AGearGameState* GearGameState = GetGameState<AGearGameState>();

		if (IsValid(GearPlayerController) && IsValid(GearGameState))
		{
			RequestPlaceRoadModuleForPlayer(GearPlayerController, RoadModuleClass, GearGameState->GetRoadStackAttachableSocket(), false);
		}
	}
}

void AGearGameMode::DestroyBuilderAndSpawnVehicle()
{
	for (APlayerState* Player : GameState->PlayerArray)
	{
		AGearPlayerState* GearPlayerState = Cast<AGearPlayerState>(Player);
		if (IsValid(GearPlayerState) && IsValid(GearPlayerState->VehicleClass))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FTransform SpawnTransform = FTransform::Identity;

			AGearVehicle* GearVehicle = GetWorld()->SpawnActor<AGearVehicle>(GearPlayerState->VehicleClass->GetAuthoritativeClass(), SpawnTransform, SpawnParams);

			AGearBuilderPawn* BuilderPawn = Cast<AGearBuilderPawn>(Player->GetPawn());
			if (IsValid(BuilderPawn) && IsValid(GearVehicle) && IsValid(BuilderPawn->Controller))
			{
				BuilderPawn->Controller->Possess(GearVehicle);
				BuilderPawn->Destroy();
			}
		}
	}
}

void AGearGameMode::StartRacing(bool bEveryPlayerPlaced)
{
	GetWorld()->GetTimerManager().ClearTimer(PlacingTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(RacingWaitTimerHandle, FTimerDelegate::CreateUObject(this, &AGearGameMode::RacingWaitTimeFinished), 3.0f, false);

	if (!bEveryPlayerPlaced)
	{
		PlaceUnplaced();
	}
	
	DestroyBuilderAndSpawnVehicle();

	UE_LOG(LogTemp, Warning, TEXT("start racing"));
	SetGearMatchState(EGearMatchState::Racing_WaitTime);
}

void AGearGameMode::RacingWaitTimeFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(RacingWaitTimerHandle);



	SetGearMatchState(EGearMatchState::Racing);
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
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGearGameMode::StartFirstPhase, UGameVariablesBFL::GV_AllPlayerJoinToGameStartDelay());

	UE_LOG(LogTemp, Warning, TEXT("all player joined"));
	SetGearMatchState(EGearMatchState::AllPlayersJoined);
}