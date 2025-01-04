// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "PlaceableSpawnPoint.generated.h"

UCLASS()
class GEAR_API APlaceableSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:	
	APlaceableSpawnPoint();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	float PreviewRotationSpeed = 15.0f;

	float PreviewRotaionOffset;
};
