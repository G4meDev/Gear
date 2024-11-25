// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GearHazardActor.generated.h"

UENUM(BlueprintType)
enum class EHazardState : uint8
{
	Preview,
	Idle,
	Enabled
};

UCLASS(Blueprintable)
class GEAR_API AGearHazardActor : public AActor
{
	GENERATED_BODY()
	
public:
	AGearHazardActor();

	virtual void SetHazardState(EHazardState State);

	UFUNCTION()
	virtual void OnRep_HazardState(EHazardState OldValue);

	void GoPreview();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	float PreviewRotationSpeed = 30.0f;

	float PreviewRotaionOffset;

	UPROPERTY(ReplicatedUsing=OnRep_HazardState, BlueprintReadWrite, EditAnywhere)
	EHazardState HazardState;
};
