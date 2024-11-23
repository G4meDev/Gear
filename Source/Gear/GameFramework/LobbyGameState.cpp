// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/LobbyGameState.h"
#include "GameFramework/LobbyPlayerController.h"

void ALobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

}

void ALobbyGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

}
