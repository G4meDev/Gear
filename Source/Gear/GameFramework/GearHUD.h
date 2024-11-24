// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GearHUD.generated.h"

class AGearPlayerState;

/**
 * 
 */
UCLASS()
class GEAR_API AGearHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowWaitingScreen();

	UFUNCTION(BlueprintImplementableEvent)
	void RemoveWaitingScreen();

	UFUNCTION(BlueprintImplementableEvent)
	void AddPlayer(AGearPlayerState* InPlayer);

	UFUNCTION(BlueprintImplementableEvent)
	void RemovePlayer(AGearPlayerState* InPlayer);
};