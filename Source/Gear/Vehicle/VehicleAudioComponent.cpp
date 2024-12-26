// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/VehicleAudioComponent.h"
#include "Vehicle/GearVehicle.h"
#include "ChaosWheeledVehicleMovementComponent.h"

constexpr auto ForwardSpeedParameter			= TEXT("ForwardSpeed");
constexpr auto WheelRotationPerSecondsParameter = TEXT("WheelRotationPerSeconds");
constexpr auto WheelRotationParameter			= TEXT("WheelRotation");

UVehicleAudioComponent::UVehicleAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVehicleAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningVehicle = Cast<AGearVehicle>(GetOwner());
	check(OwningVehicle);
}

void UVehicleAudioComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetFloatParameter(ForwardSpeedParameter				, OwningVehicle->GetChaosMovementComponent()->GetForwardSpeed() * 0.036f);
	SetFloatParameter(WheelRotationPerSecondsParameter	, FMath::Abs(OwningVehicle->GetWheelRotationSpeed(0)) / 360.0f);
	SetFloatParameter(WheelRotationParameter			, FMath::Abs(OwningVehicle->GetWheelRotation(0)));
}