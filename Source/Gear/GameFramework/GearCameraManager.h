// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "GearCameraManager.generated.h"

class ATrackSpline;
class AGearRoadModule;

/**
 * 
 */
UCLASS()
class GEAR_API AGearCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	
	ATrackSpline* TrackSpline;

	void RoadModuleStackChanged(const TArray<AGearRoadModule*> RoadModulesStack);

protected:
	virtual void BeginPlay() override;

	virtual void InitializeFor(class APlayerController* PC) override;

	virtual void Destroyed() override;
};