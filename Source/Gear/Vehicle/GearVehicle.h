// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "GearVehicle.generated.h"

class AGearGameState;
class UInputAction;
struct FInputActionInstance;

/**
 * 
 */
UCLASS()
class GEAR_API AGearVehicle : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:

	AGearVehicle();
	void Tick(float DeltaSeconds) override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool CanDrive();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* SteerActionInput;

	void Input_Steer(const FInputActionInstance& Instance);

	float SteerValue;

	AGearGameState* GearGameState;

private:
};