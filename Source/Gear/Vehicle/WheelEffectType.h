// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WheelEffectType.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UWheelEffectType : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category=Effect)
	class UNiagaraSystem* DefaultFX;

	UPROPERTY(EditDefaultsOnly, Category=Effect)
	class UNiagaraSystem* CementWetFX;

	UPROPERTY(EditDefaultsOnly, Category=Speed)
	float DefaultMinSpeed;

	UPROPERTY(EditDefaultsOnly, Category=Speed)
	float CementWetMinSpeed;


	class UNiagaraSystem* GetFX(UPhysicalMaterial* PhysMaterial, float CurrentSpeed);

protected:

	class UNiagaraSystem* GetWheelFX(TEnumAsByte<EPhysicalSurface> MaterialType);

	float GetMinSpeed(TEnumAsByte<EPhysicalSurface> MaterialType);
	
};
