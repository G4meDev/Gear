// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GearAbility.generated.h"

UCLASS()
class GEAR_API AGearAbility : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* AbilityIcon;

public:	
	AGearAbility();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
};