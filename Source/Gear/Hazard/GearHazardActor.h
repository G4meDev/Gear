// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GearHazardActor.generated.h"

UCLASS(Blueprintable)
class GEAR_API AGearHazardActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGearHazardActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
