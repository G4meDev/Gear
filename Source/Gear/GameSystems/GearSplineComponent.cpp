// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/GearSplineComponent.h"
#include "Net/UnrealNetwork.h"

UGearSplineComponent::UGearSplineComponent()
{
	SetIsReplicatedByDefault(true);
}

void UGearSplineComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGearSplineComponent, SplineCurves);
	DOREPLIFETIME(UGearSplineComponent, ReparamStepsPerSegment);
	DOREPLIFETIME(UGearSplineComponent, bStationaryEndpoints);
}