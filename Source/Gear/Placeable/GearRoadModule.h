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

	//void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	void SetPreview() override;
	void SetSelectedBy(AGearBuilderPawn* Player) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UPlaceableSocket* RoadEndSocket;

	virtual bool IsFlipable() override;

protected:
	virtual void BeginPlay() override;

};