
// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/GearVehicle.h"

#include "GameFramework/GearGameState.h"
#include "Vehicle/VehicleCamera.h"
#include "GameSystems/TrackSpline.h"
#include "ChaosVehicleMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

#define VEHICLE_TESTMAP_NAME "VehicleTestMap"


AGearVehicle::AGearVehicle()
{
	SteerValue = 0.0f;

	bAlwaysRelevant = true;
	DistanaceAlongTrack = 0;
	TargetCheckpoint = 1;
}

void AGearVehicle::BeginPlay()
{
	Super::BeginPlay();

	GearGameState = GetWorld()->GetGameState<AGearGameState>();
	check(GearGameState);

	UpdateDistanceAlongTrack();
	GearGameState->UpdateFurthestDistanceWithVehicle(this);
}

// only server
void AGearVehicle::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}

// server and owning client
void AGearVehicle::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

}

void AGearVehicle::Destroyed()
{
	Super::Destroyed();

	if (HasAuthority())
	{
		GearGameState->Vehicles.Remove(this);
	}
}

void AGearVehicle::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

#if WITH_EDITOR
	//bInTestMap = GetWorld()->GetName().Equals(VEHICLE_TESTMAP_NAME);
	bInTestMap = true;
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


	UpdateDistanceAlongTrack();

	if (HasAuthority())
	{
		if (IsOutsideTrack())
		{
			Killed();
		}
	}
}

bool AGearVehicle::CanDrive()
{
#if WITH_EDITOR
	if (bInTestMap) return true;
#endif

//	return false;

	bool bRaceStarted = IsValid(GearGameState) ? GearGameState->GearMatchState == EGearMatchState::Racing : false;

	return bRaceStarted;
}

void AGearVehicle::UpdateDistanceAlongTrack()
{
	DistanaceAlongTrack = GearGameState->TrackSpline->GetTrackDistanceAtPosition(GetActorLocation());
}

bool AGearVehicle::IsOutsideTrack() const
{
	FTransform TrackTransform = GearGameState->TrackSpline->GetTrackTransfsormAtDistance(DistanaceAlongTrack);
	return GetActorLocation().Z - TrackTransform.GetLocation().Z < -200.0f;
}

// on server
void AGearVehicle::Killed()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Killed"), *GetName());

	Destroy();
}