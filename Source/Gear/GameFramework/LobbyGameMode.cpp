// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/LobbyGameMode.h"
#include "GameFramework/LobbyPlayerController.h"
#include "GameFramework/LobbyGameState.h"
#include "GameFramework/LobbyPlayerState.h"

ALobbyGameMode::ALobbyGameMode()
{
	PlayerControllerClass = ALobbyPlayerController::StaticClass();
	GameStateClass = ALobbyGameState::StaticClass();
	PlayerStateClass = ALobbyPlayerState::StaticClass();

	bUseSeamlessTravel = true;
}

void ALobbyGameMode::KickPlayer(class ALobbyPlayerController* Player)
{
	if (IsValid(Player))
	{
		Player->NotifyKicked_Client();
		Player->Destroy();
	}
}