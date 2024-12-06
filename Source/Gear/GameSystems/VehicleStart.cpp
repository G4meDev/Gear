// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/VehicleStart.h"

UVehicleStart::UVehicleStart()
{
	PrimaryComponentTick.bCanEverTick = false;

	bOccupied = false;
}


void UVehicleStart::BeginPlay()
{
	Super::BeginPlay();

	
}


void UVehicleStart::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UVehicleStart::IsOccupied() const
{
	return bOccupied;
}

void UVehicleStart::MarkOccupied()
{
	bOccupied = true;
}

void UVehicleStart::ClearOccupied()
{
	bOccupied = false;
}