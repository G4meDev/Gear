
// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/GearVehicle.h"
#include "GameFramework/GearGameState.h"
#include "ChaosVehicleMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

#define VEHICLE_TESTMAP_NAME "VehicleTestMap"


AGearVehicle::AGearVehicle()
{
	SteerValue = 0.0f;
}

void AGearVehicle::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	bInTestMap = GetWorld()->GetName().Equals(VEHICLE_TESTMAP_NAME);
#endif
}

void AGearVehicle::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(SteerActionInput, ETriggerEvent::Triggered, this, &AGearVehicle::Input_Steer);
}

void AGearVehicle::Input_Steer(const FInputActionInstance& Instance)
{
	SteerValue = Instance.GetValue().Get<float>();
}

void AGearVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	GetVehicleMovementComponent()->SetSteeringInput(SteerValue);

	if (CanDrive())
	{
		GetVehicleMovementComponent()->SetThrottleInput(1.0f);
	}
}

bool AGearVehicle::CanDrive()
{
#if WITH_EDITOR
	if (bInTestMap) return true;
#endif

	bool RaceStarted = IsValid(GearGameState) ? GearGameState->GearMatchState == EGearMatchState::Racing : false;
	return RaceStarted;
}