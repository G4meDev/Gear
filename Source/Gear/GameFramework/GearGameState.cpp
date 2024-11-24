// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearGameState.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearPlayerController.h"

AGearGameState::AGearGameState()
{

}

void AGearGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			AGearPlayerController* LobbyController = Cast<AGearPlayerController>(PlayerController);

			if (IsValid(LobbyController))
			{
				AGearPlayerState* GearPlayer = Cast<AGearPlayerState>(PlayerState);
				if (IsValid(GearPlayer))
				{
					LobbyController->OnNewPlayer(GearPlayer);
				}
			}
		}
	}
}

void AGearGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			AGearPlayerController* LobbyController = Cast<AGearPlayerController>(PlayerController);

			if (IsValid(LobbyController))
			{
				AGearPlayerState* GearPlayer = Cast<AGearPlayerState>(PlayerState);
				if (IsValid(GearPlayer))
				{
					LobbyController->OnRemovePlayer(GearPlayer);
				}
			}
		}
	}
}
