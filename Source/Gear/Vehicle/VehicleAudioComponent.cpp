// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/VehicleAudioComponent.h"
#include "Vehicle/GearVehicle.h"
#include "ChaosWheeledVehicleMovementComponent.h"

constexpr auto VehicleForwardSpeedParameter = TEXT("VehicleForwardSpeed");

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

	SetFloatParameter(VehicleForwardSpeedParameter, OwningVehicle->GetChaosMovementComponent()->GetForwardSpeed() * 0.036f);
}