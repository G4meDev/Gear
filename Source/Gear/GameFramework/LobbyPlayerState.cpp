// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/LobbyPlayerState.h"
#include "GameFramework/LobbyplayerController.h"

void ALobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();


}

void ALobbyPlayerState::Destroyed()
{
	Super::Destroyed();


}

void ALobbyPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	OnPlayerNameChanged.Broadcast();
}
