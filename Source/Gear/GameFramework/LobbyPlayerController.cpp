// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/LobbyPlayerController.h"
#include "GameFramework/GearGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/LobbyPlayerState.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		UGearGameInstance* GearGameIntance = Cast<UGearGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		Server_SetPlayerName(GearGameIntance->GetPlayerName());
	}

}

void ALobbyPlayerController::Server_SetPlayerName_Implementation(const FString& PlayerName)
{
	PlayerState->SetPlayerName(PlayerName);
}

void ALobbyPlayerController::NotifyNewPlayer(APlayerState* InPlayer)
{
	ALobbyPlayerState* LobbyPlayer = Cast<ALobbyPlayerState>(InPlayer);
	if (IsValid(LobbyPlayer))
	{
		OnAddPlayer(LobbyPlayer);
	}
}

void ALobbyPlayerController::NotifyRemovePlayer(APlayerState* InPlayer)
{
	ALobbyPlayerState* LobbyPlayer = Cast<ALobbyPlayerState>(InPlayer);
	if (IsValid(LobbyPlayer))
	{
		OnRemovePlayer(LobbyPlayer);
	}	
}
