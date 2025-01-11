// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Placeable/GearPlaceable.h"
#include "GearHazard.generated.h"


UCLASS(Blueprintable)
class GEAR_API AGearHazard : public AGearPlaceable
{
	GENERATED_BODY()
	
public:
	AGearHazard();

	void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;
	
	void SetPreview() override;
	void SetSelectedBy(AGearBuilderPawn* Player) override;



protected:
	virtual void BeginPlay() override;

#if WITH_EDITORONLY_DATA

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bDirty = false;

	float RotationPivotHeightOffset = -200.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SelectionHitboxPadding = 50.0f;

#endif

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION(BlueprintNativeEvent)
	void GetHazardBounds(FVector& Min, FVector& Max);

#endif
};
