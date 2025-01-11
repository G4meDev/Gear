// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HazardSocketMarker.generated.h"

UCLASS()
class GEAR_API AHazardSocketMarker : public AActor
{
	GENERATED_BODY()
	
public:	
	AHazardSocketMarker();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};