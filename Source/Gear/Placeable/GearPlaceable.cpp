// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearPlaceable.h"
#include "Net/UnrealNetwork.h"

AGearPlaceable::AGearPlaceable()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);
}

void AGearPlaceable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearPlaceable, OwningPlayer);
}

void AGearPlaceable::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGearPlaceable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGearPlaceable::RoundReset()
{
	RoundRestBlueprintEvent();
}

bool AGearPlaceable::HasOwningPlayer() const
{
	return OwningPlayer != nullptr;
}

void AGearPlaceable::OnRep_OwningPlayer()
{
	if (OwningPlayer == nullptr)
	{
		SetPreview();
	}
	else
	{
		SetSelectedBy(OwningPlayer);
	}
}