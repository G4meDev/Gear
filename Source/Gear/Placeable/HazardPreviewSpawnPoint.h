// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "HazardPreviewSpawnPoint.generated.h"

UCLASS()
class GEAR_API AHazardPreviewSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:	
	AHazardPreviewSpawnPoint();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
