// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "VehicleAudioComponent.generated.h"

class AGearVehicle;

/**
 * 
 */
UCLASS(Blueprintable)
class GEAR_API UVehicleAudioComponent : public UAudioComponent
{
	GENERATED_BODY()

public:
	UVehicleAudioComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:

	AGearVehicle* OwningVehicle;
};