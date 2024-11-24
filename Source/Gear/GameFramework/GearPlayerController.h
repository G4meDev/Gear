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

	virtual void BeginPlay() override;


public:

	AGearHUD* GearHUD;

	void OnNewPlayer(AGearPlayerState* GearPlayer);
	void OnRemovePlayer(AGearPlayerState* GearPlayer);
};
