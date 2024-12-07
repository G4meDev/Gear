
// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/GearVehicle.h"
#include "GameFramework/SpringarmComponent.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/GearGameState.h"
#include "ChaosVehicleMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

#define VEHICLE_TESTMAP_NAME "VehicleTestMap"


AGearVehicle::AGearVehicle()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetupAttachment(GetMesh());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	SteerValue = 0.0f;

	bAlwaysRelevant = true;

}

void AGearVehicle::BeginPlay()
{
	Super::BeginPlay();

	GearGameState = GetWorld()->GetGameState<AGearGameState>();
}

void AGearVehicle::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

#if WITH_EDITOR
	bInTestMap = GetWorld()->GetName().Equals(VEHICLE_TESTMAP_NAME);
#endif

	if (IsLocallyControlled())
	{
		UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
		Input->BindAction(SteerActionInput, ETriggerEvent::Triggered, this, &AGearVehicle::Input_Steer);
#if WITH_EDITOR
		if (bInTestMap)
		{
			Input->BindAction(ThrottleActionInput, ETriggerEvent::Triggered, this, &AGearVehicle::Input_Throttle);
			Input->BindAction(BrakeActionInput, ETriggerEvent::Triggered, this, &AGearVehicle::Input_Brake);
		}
#endif
	}
}

void AGearVehicle::Input_Steer(const FInputActionInstance& Instance)
{
	SteerValue = Instance.GetValue().Get<float>();
}

#if WITH_EDITOR
void AGearVehicle::Input_Throttle(const FInputActionInstance& Instance)
{
	ThrottleValue = Instance.GetValue().Get<float>();
}

void AGearVehicle::Input_Brake(const FInputActionInstance& Instance)
{
	BrakeValue = Instance.GetValue().Get<float>();
}
#endif

void AGearVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (IsLocallyControlled())
	{
		GetVehicleMovementComponent()->SetSteeringInput(SteerValue);

		if (CanDrive())
		{
			GetVehicleMovementComponent()->SetThrottleInput(1.0f);
		}

#if WITH_EDITOR
		if (bInTestMap)
		{
			GetVehicleMovementComponent()->SetThrottleInput(ThrottleValue);
			GetVehicleMovementComponent()->SetBrakeInput(BrakeValue);
		}
#endif
	}
}

bool AGearVehicle::CanDrive()
{
#if WITH_EDITOR
	if (bInTestMap) return true;
#endif

	return false;

// 	bool bRaceStarted = IsValid(GearGameState) ? GearGameState->GearMatchState == EGearMatchState::Racing : false;
// 
// 	return bRaceStarted;
}