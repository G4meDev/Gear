// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GearAbility.generated.h"

UCLASS()
class GEAR_API AGearAbility : public AActor
{
	GENERATED_BODY()
	
public:	
	AGearAbility();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};