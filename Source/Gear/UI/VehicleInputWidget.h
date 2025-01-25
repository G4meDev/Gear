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
	UPROPERTY(BlueprintReadOnly)
	class AGearVehicle* OwningVehicle;
};