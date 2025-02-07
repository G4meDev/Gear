// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/LobbyGameState.h"
#include "GameFramework/LobbyPlayerController.h"
#include "GameFramework/LobbyPlayerState.h"
#include "GameSystems/LobbyPlayerPlatform.h"
#include "Kismet/GameplayStatics.h"

ALobbyGameState::ALobbyGameState()
{

}

void ALobbyGameState::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> PlatformActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyPlayerPlatform::StaticClass(), PlatformActors);

	PlayerPlatforms.Reserve(4);
	for (AActor* PlatformActor : PlatformActors)
	{
		ALobbyPlayerPlatform* PlayerPlatform = Cast<ALobbyPlayerPlatform>(PlatformActor);
		if (IsValid(PlayerPlatform))
		{
			PlayerPlatforms.Add(PlayerPlatform);
		}
	}

	PlayerPlatforms.StableSort([](const ALobbyPlayerPlatform& P1, const ALobbyPlayerPlatform& P2)
		{
			return P1.GetPlatformIndex() < P2.GetPlatformIndex();
		});

	ReconstructPlayersPlatform();
}

void ALobbyGameState::GetInUseColors(TArray<EPlayerColorCode>& InUseColors)
{
	for (APlayerState* Player : PlayerArray)
	{
		ALobbyPlayerState* LobbyPlayer = Cast<ALobbyPlayerState>(Player);
		if (IsValid(LobbyPlayer))
		{
			InUseColors.Add(LobbyPlayer->ColorCode);
		}
	}
}

void ALobbyGameState::AssignNewColorToPlayer(APlayerState* Player)
{
	ALobbyPlayerState* LobbyPlayer = Cast<ALobbyPlayerState>(Player);
	if (IsValid(LobbyPlayer))
	{
		TArray<EPlayerColorCode> InUseColors;
		GetInUseColors(InUseColors);
		InUseColors.RemoveSingle(LobbyPlayer->ColorCode);

		for (uint8 i = 0; i < static_cast<uint8>(EPlayerColorCode::MAX_COLOR); i++)
		{
			EPlayerColorCode ColorCode = static_cast<EPlayerColorCode>(i);

			if (!InUseColors.Contains(ColorCode))
			{
				LobbyPlayer->ColorCode = ColorCode;
				LobbyPlayer->OnRep_ColorCode();
				return;
			}
		}
	}
}

void ALobbyGameState::RequestColorChangeForPlayer(ALobbyPlayerController* PC, EPlayerColorCode Color)
{
	ALobbyPlayerState* Player = Cast<ALobbyPlayerState>(PC->PlayerState);
	if (!IsValid(Player))
	{
		return;
	}

	TArray<EPlayerColorCode> InUseColors;
	GetInUseColors(InUseColors);

	if (!InUseColors.Contains(Color))
	{
		Player->ColorCode = Color;
		Player->OnRep_ColorCode();
	}

	else
	{
		PC->ColorChange_RollBack(Player->ColorCode);
	}
}

TArray<ALobbyPlayerState*> ALobbyGameState::GetLobbyPlayers()
{
	TArray<ALobbyPlayerState*> LobbyPlayerStates;
	for (APlayerState* PlayerState : PlayerArray)
	{
		ALobbyPlayerState* LobbyPlayerState = Cast<ALobbyPlayerState>(PlayerState);
		if (IsValid(LobbyPlayerState))
		{
			LobbyPlayerStates.Add(LobbyPlayerState);
		}
	}

	return LobbyPlayerStates;
}

TArray<ALobbyPlayerState*> ALobbyGameState::GetJoinTimeSortedLobbyPlayers()
{
	TArray<ALobbyPlayerState*> LobbyPlayerStates = GetLobbyPlayers();
	LobbyPlayerStates.StableSort(ALobbyPlayerState::SortJoinTimePredicate);
	
	return LobbyPlayerStates;
}

void ALobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	ReconstructPlayersPlatform();

	if (HasAuthority())
	{
		AssignNewColorToPlayer(PlayerState);
	}


	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			ALobbyPlayerController* LobbyController = Cast<ALobbyPlayerController>(PlayerController);

			if (IsValid(LobbyController))
			{
				LobbyController->NotifyNewPlayer(PlayerState);
			}
		}
	}
}

void ALobbyGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	ALobbyPlayerState* LobbyPlayerState = Cast<ALobbyPlayerState>(PlayerState);
	if (IsValid(LobbyPlayerState))
	{
		LobbyPlayerState->PlayerJoinTime = GetServerWorldTimeSeconds();
	}

	ReconstructPlayersPlatform();

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			ALobbyPlayerController* LobbyController = Cast<ALobbyPlayerController>(PlayerController);

			if (IsValid(LobbyController))
			{
				LobbyController->NotifyRemovePlayer(PlayerState);
			}
		}
	}
}

void ALobbyGameState::ReconstructPlayersPlatform()
{
	TArray<ALobbyPlayerState*> LobbyPlayers = GetJoinTimeSortedLobbyPlayers();

	for (int i = 0; i < PlayerPlatforms.Num(); i++)
	{
		ALobbyPlayerPlatform* PlayerPlatform = PlayerPlatforms[i];
		if (IsValid(PlayerPlatform))
		{
			ALobbyPlayerState* PlayerState = i < LobbyPlayers.Num() ? LobbyPlayers[i] : nullptr;
			PlayerPlatform->SetOwningPlayer(PlayerState);
		}
	}
}