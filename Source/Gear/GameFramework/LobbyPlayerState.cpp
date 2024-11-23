// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/LobbyPlayerState.h"
#include "GameFramework/LobbyplayerController.h"

void ALobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			ALobbyPlayerController* LobbyController = Cast<ALobbyPlayerController>(PlayerController);

			if (IsValid(LobbyController))
			{
				LobbyController->NotifyNewPlayer(this);
			}
		}
	}


}

void ALobbyPlayerState::Destroyed()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			ALobbyPlayerController* LobbyController = Cast<ALobbyPlayerController>(PlayerController);

			if (IsValid(LobbyController))
			{
				LobbyController->NotifyRemovePlayer(this);
			}
		}
	}

	Super::Destroyed();
}

void ALobbyPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	OnPlayerNameChanged.Broadcast();
}
