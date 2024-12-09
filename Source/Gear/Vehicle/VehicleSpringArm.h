// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "VehicleSpringArm.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UVehicleSpringArm : public USpringArmComponent
{
	GENERATED_BODY()
	
public:
	void TeleportToDesireLocation();
	
};
