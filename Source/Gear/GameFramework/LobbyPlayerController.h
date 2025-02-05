// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GearTypes.h"
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

	virtual void PostNetInit() override;
	void BeginPlay() override;

	void OnRep_PlayerState() override;

	void AddLobbyMenu();
	
	UFUNCTION(Server, Reliable)
	void Server_SetPlayerName(const FString& PlayerName);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAddPlayer(ALobbyPlayerState* InPlayer);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRemovePlayer(ALobbyPlayerState* InPlayer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UUserWidget> LobbyWidgetClass;

public:

	void NotifyNewPlayer(APlayerState* InPlayer);
	void NotifyRemovePlayer(APlayerState* InPlayer);

	UFUNCTION(Server, Reliable)
	void RequestColorChange(EPlayerColorCode Color);

	UFUNCTION(Client, Reliable)
	void ColorChange_RollBack(EPlayerColorCode Color);

	UFUNCTION(BlueprintCallable)
	void TryChangeColor(EPlayerColorCode Color);

	ALobbyPlayerState* GetLobbyPlayerState();
};
