// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Placeable/GearPlaceable.h"
#include "GearRoadModule.generated.h"

class UPlaceableSocket;

/**
 * 
 */
UCLASS()
class GEAR_API AGearRoadModule : public AGearPlaceable
{
	GENERATED_BODY()
	
public:
	AGearRoadModule();

	void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	void SetPreview() override;
	void SetSelectedBy(AGearPlayerState* Player) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UPlaceableSocket* RoadEndSocket;

protected:
	virtual void BeginPlay() override;

};