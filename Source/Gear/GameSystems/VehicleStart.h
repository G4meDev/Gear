// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "VehicleStart.generated.h"


UCLASS( ClassGroup=(Gear), meta=(BlueprintSpawnableComponent) )
class GEAR_API UVehicleStart : public USceneComponent
{
	GENERATED_BODY()

public:	
	UVehicleStart();

protected:
	virtual void BeginPlay() override;

	bool bOccupied;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsOccupied() const;

	void MarkOccupied();
	void ClearOccupied();
};
