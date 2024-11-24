// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GearPlayerController.generated.h"

class APlayerState;

/**
 * 
 */
UCLASS()
class GEAR_API AGearPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	UFUNCTION(BlueprintImplementableEvent)
	void OnAddPlayer(AGearPlayerState* InPlayer);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRemovePlayer(AGearPlayerState* InPlayer);

public:

	void NotifyNewPlayer(APlayerState* InPlayer);
	void NotifyRemovePlayer(APlayerState* InPlayer);
};
