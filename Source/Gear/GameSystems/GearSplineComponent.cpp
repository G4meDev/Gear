// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/GearSplineComponent.h"
#include "Net/UnrealNetwork.h"

UGearSplineComponent::UGearSplineComponent()
{
	SetIsReplicatedByDefault(false);
}

FSplineCurves UGearSplineComponent::GetSplineCurves()
{
	return SplineCurves;
}

void UGearSplineComponent::SetSplineCurves(const FSplineCurves& InSplineCurves)
{
	SplineCurves = InSplineCurves;
	UpdateSpline();
}

//void UGearSplineComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(UGearSplineComponent, SplineCurves);
//}