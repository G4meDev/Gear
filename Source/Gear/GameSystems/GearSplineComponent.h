// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GearSplineComponent.generated.h"


UCLASS()
class GEAR_API UGearSplineComponent : public USplineComponent
{
	GENERATED_BODY()


public:

	UGearSplineComponent();

public:
	FSplineCurves GetSplineCurves();

	void SetSplineCurves(const FSplineCurves& InSplineCurves);

	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
