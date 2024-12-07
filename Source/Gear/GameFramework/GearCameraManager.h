// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "GearCameraManager.generated.h"

class AGearRoadModule;
class AGearGameState;

/**
 * 
 */
UCLASS()
class GEAR_API AGearCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	
	virtual void Tick( float DeltaSeconds ) override;

protected:
	virtual void BeginPlay() override;

	virtual void InitializeFor(class APlayerController* PC) override;

	virtual void Destroyed() override;

	AGearGameState* GearGameState;
};