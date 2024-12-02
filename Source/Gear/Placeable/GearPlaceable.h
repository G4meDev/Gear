
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystems/RoundResetable.h"
#include "GameFramework/GearTypes.h"
#include "GearPlaceable.generated.h"

class AGearBuilderPawn;
class UBoxComponent;
class UStaticMeshComponent;
class APlaceableSpawnPoint;

UCLASS(Blueprintable)
class GEAR_API AGearPlaceable : public AActor, public IRoundResetable
{
	GENERATED_BODY()
	
public:	
	AGearPlaceable();

	void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void RoundReset() override;

	virtual void SetPreview();

	virtual void SetSelectedBy(AGearBuilderPawn* Player);

	UFUNCTION(BlueprintImplementableEvent)
	void RoundRestBlueprintEvent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPlaceableSocketType SocketType;

	UPROPERTY(ReplicatedUsing=OnRep_OwningPlayer)
	AGearBuilderPawn* OwningPlayer;

	bool HasOwningPlayer() const;

	UFUNCTION()
	void OnRep_OwningPlayer();


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* SelectionHitbox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* SelectionIndicator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* PreviewRotationPivot;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PreviewScale;

	UMaterialInstanceDynamic* SelectionIndicatorMaterial;

	void AttachToSpawnPoint(APlaceableSpawnPoint* SpawnPoint);

	UFUNCTION(BlueprintPure)
	virtual bool IsFlipable();

	void Flip();

	bool bFliped;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	EPlaceableState PlaceableState;

	void SelectionBoxClicked();

	UFUNCTION()
	void OnSelectionBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	UFUNCTION()
	void OnSelectionBoxTouched(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);
};