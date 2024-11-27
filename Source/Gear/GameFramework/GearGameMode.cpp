// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearGameMode.h"
#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearGameState.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearBuilderPawn.h"
#include "Hazard/GearHazardActor.h"
#include "Hazard/HazardPreviewSpawnPoint.h"

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

	if (ShouldAbort())
	{
		AbortMatch();
	}
}


void AGearGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	GearMatchState = EGearMatchState::Ended;
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
		AGearHazardActor* HazardActor = GetWorld()->SpawnActor<AGearHazardActor>(AvaliableHazards[0].Class, SpawnPoint->GetTransform());
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
	if (IsValid(Hazard) && IsValid(Player) && Hazard->OwningPlayer == nullptr)
	{
		Player->SetSelectedHazard(Hazard);
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

bool AGearGameMode::ReadyToEndMatch_Implementation()
{
	return false;
}

void AGearGameMode::StartSelectingPieces()
{
	UE_LOG(LogTemp, Warning, TEXT("Selecting pieces!"));

	GearMatchState = EGearMatchState::SelectingPeices;

	SpawnNewBuilderPawns();
	SpawnNewHazzards();
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
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGearGameMode::StartFirstPhase, StartDelayAfterAllJoined);

		GearMatchState = EGearMatchState::AllPlayersJoined;
	}
}