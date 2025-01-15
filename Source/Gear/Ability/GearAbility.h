// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GearAbility.generated.h"

UCLASS()
class GEAR_API AGearAbility : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* AbilityIcon;

	UPROPERTY(ReplicatedUsing=OnRep_OwningVehicle)
	class AGearVehicle* OwningVehice;

public:	
	AGearAbility();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void SetOwningVehicle(AGearVehicle* InOwningVehice);


	UFUNCTION()
	virtual void OnRep_OwningVehicle();

protected:
};