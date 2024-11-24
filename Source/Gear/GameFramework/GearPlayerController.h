// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GearPlayerController.generated.h"

class AGearPlayerState;
class AGearHUD;

/**
 * 
 */
UCLASS()
class GEAR_API AGearPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	AGearPlayerController();

	virtual void BeginPlay() override;



public:

	bool IsReady;

	UPROPERTY()
	AGearHUD* GearHUD;

	UFUNCTION(Client, Unreliable)
	void PeekClientIsReady();

	UFUNCTION(Server, Unreliable)
	void RespondClientIsReady();

	void OnNewPlayer(AGearPlayerState* GearPlayer);
	void OnRemovePlayer(AGearPlayerState* GearPlayer);

	UFUNCTION(Client, Reliable)
	void AllPlayersJoined();

	UFUNCTION(Client, Reliable)
	void MatchStarted();

};