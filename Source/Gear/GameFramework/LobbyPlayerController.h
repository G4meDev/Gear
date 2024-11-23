// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

class ALobbyPlayerState;

/**
 * 
 */
UCLASS()
class GEAR_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;
	
	UFUNCTION(Server, Reliable)
	void Server_SetPlayerName(const FString& PlayerName);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAddPlayer(ALobbyPlayerState* InPlayer);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRemovePlayer(ALobbyPlayerState* InPlayer);

public:
	void NotifyNewPlayer(APlayerState* InPlayer);
	void NotifyRemovePlayer(APlayerState* InPlayer);
};
