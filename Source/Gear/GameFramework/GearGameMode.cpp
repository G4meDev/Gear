// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearGameMode.h"
#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearGameState.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearBuilderPawn.h"
#include "Hazard/GearHazardActor.h"
#include "Hazard/HazardPreviewSpawnPoint.h"
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

	if (!LoadHazards())
	{
		UE_LOG(LogGameMode, Error, TEXT("Not enough hazards found"));
		AbortMatch();
	}

	if (!LoadHazardPreviewSpawnPoints())
	{
		UE_LOG(LogGameMode, Error, TEXT("Not enough hazards preview spawn point found"));
		AbortMatch();
	}
}

void AGearGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GearMatchState == EGearMatchState::SelectingPeices)
	{
		if (IsEveryPlayerSelectedPieces())
		{
			StartPlaceingPieces(true);
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
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController)
		{
			AGearPlayerController* GearController = Cast<AGearPlayerController>(PlayerController);
			if (IsValid(GearController))
			{
				AGearBuilderPawn* BuilderPawn = GetWorld()->SpawnActor<AGearBuilderPawn>(DefaultPawnClass);
				GearController->Possess(BuilderPawn);
			}
		}
	}
}

void AGearGameMode::SpawnNewHazzards()
{
	PreviewHazards.Empty(5);

	for (AHazardPreviewSpawnPoint* SpawnPoint : HazardPreviewSpawnPoints)
	{
		// TODO: implement hazard spawning logic, for now spawn randomly
		TSubclassOf<AGearHazardActor>& SpawnClass = AvaliableHazards[ FMath::RandRange(0, AvaliableHazards.Num()-1) ].Class;

		AGearHazardActor* HazardActor = GetWorld()->SpawnActor<AGearHazardActor>(SpawnClass, SpawnPoint->GetTransform());
		HazardActor->SetPreview();
		
		PreviewHazards.Add(HazardActor);
	}
}

bool AGearGameMode::LoadHazards()
{
	AvaliableHazards.Empty();

	if (IsValid(HazardSpawnRulesDataTable))
	{
		TArray<FHazardDescription*> RawHazards; 
		HazardSpawnRulesDataTable->GetAllRows<FHazardDescription>("Reading hazard rules", RawHazards);

		for (FHazardDescription* Desc : RawHazards)
		{
			TSubclassOf<AGearHazardActor> HazardClass = StaticLoadClass(AGearHazardActor::StaticClass(), nullptr, *Desc->ClassPath);
			if (IsValid(HazardClass))
			{
				FHazardDescription NewDesc = *Desc;
				NewDesc.Class = HazardClass;
				
				AvaliableHazards.Add(NewDesc);
			}
		}

		if (AvaliableHazards.Num() == 0)
		{
			return false;
		}

		return true;
	}

	return false;
}

bool AGearGameMode::LoadHazardPreviewSpawnPoints()
{
	HazardPreviewSpawnPoints.Empty(5);
	
	TArray<AActor*> SpawnActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHazardPreviewSpawnPoint::StaticClass(), SpawnActors);

	for (AActor* Actor : SpawnActors)
	{
		AHazardPreviewSpawnPoint* HazardPreviewSpawnPoint = Cast<AHazardPreviewSpawnPoint>(Actor);
		if (IsValid(HazardPreviewSpawnPoint))
		{
			HazardPreviewSpawnPoints.Add(HazardPreviewSpawnPoint);
		}
	}

	return HazardPreviewSpawnPoints.Num() == 5;
}

void AGearGameMode::RequestSelectingHazardForPlayer_Implementation(AGearHazardActor* Hazard, AGearPlayerState* Player)
{
	if (GearMatchState == EGearMatchState::SelectingPeices && IsValid(Hazard) && IsValid(Player) && !Hazard->HasOwningPlayer())
	{
		Player->SetSelectedHazard(Hazard);
	}
}

void AGearGameMode::SetGearMatchState(EGearMatchState State)
{
	GearMatchState = State;

	AGearGameState* GearGameState = GetGameState<AGearGameState>();
	if (GearGameState)
	{
		GearGameState->LastStateChangeTime = GearGameState->GetServerWorldTimeSeconds();
		GearGameState->GearMatchState = State;
	}
}

void AGearGameMode::HandleMatchAborted()
{
	Super::HandleMatchAborted();

	UE_LOG(LogTemp, Error, TEXT("Match Aborted!"));
}

void AGearGameMode::StartFirstPhase()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController)
		{
			AGearPlayerController* GearController = Cast<AGearPlayerController>(PlayerController);

			if (IsValid(GearController))
			{
				GearController->MatchStarted();
			}
		}
	}
	
	StartSelectingPieces();
}

void AGearGameMode::AssignPiecesToUnowningPlayers()
{
	TArray<AGearPlayerState*> UnowningPlayers;
	TArray<AGearHazardActor*> UnownedHazards;

	for (APlayerState* Player : GameState->PlayerArray)
	{
		AGearPlayerState* GearPlayer = Cast<AGearPlayerState>(Player);
		if (IsValid(GearPlayer) && !GearPlayer->HasSelectedHazard())
		{
			UnowningPlayers.Add(GearPlayer);
		}
	}

	for (AGearHazardActor* Hazard : PreviewHazards)
	{
		if (!Hazard->HasOwningPlayer())
		{
			UnownedHazards.Add(Hazard);
		}
	}

	for (int i = 0; i < UnowningPlayers.Num(); i++)
	{
		check(i < UnownedHazards.Num());

		UnowningPlayers[i]->SetSelectedHazard(UnownedHazards[i]);
	}
}

bool AGearGameMode::ReadyToEndMatch_Implementation()
{
	return false;
}

void AGearGameMode::StartSelectingPieces()
{
	UE_LOG(LogTemp, Warning, TEXT("Selecting pieces!"));

	SetGearMatchState(EGearMatchState::SelectingPeices);

	SpawnNewBuilderPawns();
	SpawnNewHazzards();

	GetWorld()->GetTimerManager().SetTimer(SelectingPiecesTimerHandle, FTimerDelegate::CreateUObject(this, &AGearGameMode::StartPlaceingPieces, false), UGameVariablesBFL::GV_PieceSelectionTimeLimit(), false);
}

bool AGearGameMode::IsEveryPlayerSelectedPieces()
{
	for (APlayerState* Player : GameState->PlayerArray)
	{
		AGearPlayerState* GearPlayer = Cast<AGearPlayerState>(Player);
		if (IsValid(GearPlayer) && !GearPlayer->HasSelectedHazard())
		{
			return false;
		}
	}

	return true;
}

void AGearGameMode::StartPlaceingPieces(bool bEveryPlayerIsReady)
{
	if (!bEveryPlayerIsReady)
	{
		AssignPiecesToUnowningPlayers();
	}

	SetGearMatchState(EGearMatchState::PlacingPieces);
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
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController)
		{
			AGearPlayerController* GearController = Cast<AGearPlayerController>(PlayerController);
			if (IsValid(GearController) && !GearController->IsReady)
			{
				EveryClientIsReady = false;
				GearController->PeekClientIsReady();
			}
		}
	}
	
	return NumTravellingPlayers == 0 && NumPlayers > 1 && EveryClientIsReady;
}

void AGearGameMode::AllPlayerJoined()
{
	if (GearMatchState == EGearMatchState::WaitingForPlayerToJoin)
	{
		UE_LOG(LogTemp, Warning, TEXT("all player joined"));

		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* const PlayerController = Iterator->Get();
			if (PlayerController)
			{
				AGearPlayerController* GearController = Cast<AGearPlayerController>(PlayerController);

				if (IsValid(GearController))
				{
					GearController->AllPlayersJoined();
				}
			}
		}

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGearGameMode::StartFirstPhase, UGameVariablesBFL::GV_AllPlayerJoinToGameStartDelay());

		SetGearMatchState(EGearMatchState::AllPlayersJoined);
	}
}