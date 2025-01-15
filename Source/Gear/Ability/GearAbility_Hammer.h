// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/GearAbility.h"
#include "GearAbility_Hammer.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API AGearAbility_Hammer : public AGearAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* HammerMesh;
	
public:
	AGearAbility_Hammer();

	virtual void OnRep_OwningVehicle() override;
};
