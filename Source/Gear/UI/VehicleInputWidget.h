// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/GearBaseWidget.h"
#include "VehicleInputWidget.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UVehicleInputWidget : public UGearBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void AbilityChanged(class AGearAbility* Ability);

	UPROPERTY(BlueprintReadOnly)
	class AGearVehicle* OwningVehicle;
};