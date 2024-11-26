// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/LobbyGameState.h"
#include "GameFramework/LobbyPlayerController.h"
#include "GameFramework/LobbyPlayerState.h"

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
	}

	else
	{
		PC->ColorChange_RollBack(Player->ColorCode);
	}
}

void ALobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

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

