// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VehicleInputWidget.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UVehicleInputWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void AbilityChanged(class AGearAbility* Ability);
};