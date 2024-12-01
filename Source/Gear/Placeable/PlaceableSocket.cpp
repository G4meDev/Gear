// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/PlaceableSocket.h"
#include "Net/UnrealNetwork.h"

UPlaceableSocket::UPlaceableSocket()
{
	PrimaryComponentTick.bCanEverTick = true;

	Occupied = false;
}

void UPlaceableSocket::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlaceableSocket, Occupied);
}

void UPlaceableSocket::BeginPlay()
{
	Super::BeginPlay();

	
}


void UPlaceableSocket::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UPlaceableSocket::IsOccupied() const
{
	return Occupied;
}

void UPlaceableSocket::MarkOccupied()
{
	Occupied = true;
}