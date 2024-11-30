// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Placeable/GearPlaceable.h"
#include "GearHazard.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class AGearPlayerState;

UENUM(BlueprintType)
enum class EHazardState : uint8
{
	Preview,
	Selected,
	Idle,
	Enabled
};

UCLASS(Blueprintable)
class GEAR_API AGearHazard : public AGearPlaceable
{
	GENERATED_BODY()
	
public:
	AGearHazard();

	void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;
	
	void SetPreview();
	void SetSelectedBy(AGearPlayerState* Player);

	UFUNCTION()
	void OnRep_OwningPlayer();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* SelectionHitbox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* SelectionIndicator;

	UMaterialInstanceDynamic* SelectionIndicatorMaterial;

	UPROPERTY(ReplicatedUsing=OnRep_OwningPlayer)
	AGearPlayerState* OwningPlayer;

	bool HasOwningPlayer() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	float PreviewRotationSpeed = 30.0f;

	float PreviewRotaionOffset;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	EHazardState HazardState;

	void SelectionBoxClicked();

	UFUNCTION()
	void OnSelectionBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	UFUNCTION()
	void OnSelectionBoxTouched(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);
};
