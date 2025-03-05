// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "VehicleSkidmarkComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Gear), meta = (BlueprintSpawnableComponent))
class GEAR_API UVehicleSkidmarkComponent : public USceneComponent
{
	GENERATED_BODY()
	
public:
	UVehicleSkidmarkComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;


protected:
	
	UPROPERTY(EditDefaultsOnly, Category="Effect")
	class UNiagaraSystem* DefaultEffect;

	UPROPERTY(EditDefaultsOnly, Category="Effect")
	class UNiagaraSystem* CementEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	int32 WheelIndex;

	UPROPERTY()
	class AGearVehicle* OwningVehicle;

	UPROPERTY()
	class UNiagaraComponent* NiagaraComponent;
};