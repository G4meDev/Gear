// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Placeable/GearPlaceable.h"
#include "GearHazard.generated.h"


UCLASS(Blueprintable)
class GEAR_API AGearHazard : public AGearPlaceable
{
	GENERATED_BODY()
	
public:
	AGearHazard();

	void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;
	
	void SetPreview() override;
	void SetSelectedBy(AGearBuilderPawn* Player) override;



protected:
	virtual void BeginPlay() override;


};
