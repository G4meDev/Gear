
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystems/RoundResetable.h"
#include "GameFramework/GearTypes.h"
#include "GearPlaceable.generated.h"

UCLASS(Blueprintable , abstract)
class GEAR_API AGearPlaceable : public AActor, public IRoundResetable
{
	GENERATED_BODY()
	
public:	
	AGearPlaceable();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void RoundReset() override;

	UFUNCTION(BlueprintImplementableEvent)
	void RoundRestBlueprintEvent();


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPlaceableType Type;
};