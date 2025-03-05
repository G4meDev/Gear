// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/WheelEffectType.h"
#include "GameFramework/GearTypes.h"

class UNiagaraSystem* UWheelEffectType::GetFX(UPhysicalMaterial* PhysMaterial, float CurrentSpeed)
{
	UNiagaraSystem* WheelFX = nullptr;
	
	if (IsValid(PhysMaterial))
	{
		EPhysicalSurface SurfaceType = PhysMaterial->SurfaceType;

		WheelFX = GetWheelFX(SurfaceType);
		if (WheelFX)
		{
			const float MinSpeed = GetMinSpeed(SurfaceType);
			if (CurrentSpeed < MinSpeed)
			{
				WheelFX = nullptr;
			}
		}
	}

	return WheelFX;
}

class UNiagaraSystem* UWheelEffectType::GetWheelFX(TEnumAsByte<EPhysicalSurface> MaterialType)
{
	UNiagaraSystem* WheelFX = nullptr;

	switch (MaterialType)
	{
	case SURFACE_Default:				WheelFX = DefaultFX; break;
	case SURFACE_CementWet:				WheelFX = CementWetFX; break;
	default:							WheelFX = nullptr; break;
	}

	return WheelFX;
}

float UWheelEffectType::GetMinSpeed(TEnumAsByte<EPhysicalSurface> MaterialType)
{
	float MinSpeed = 0.0f;

	switch (MaterialType)
	{
	case SURFACE_Default:				MinSpeed = DefaultMinSpeed; break;
	case SURFACE_CementWet:				MinSpeed = CementWetMinSpeed; break;
	default:							MinSpeed = 0.0f; break;
	}

	return MinSpeed;
}