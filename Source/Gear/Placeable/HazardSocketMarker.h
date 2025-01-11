// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HazardSocketMarker.generated.h"

UCLASS()
class GEAR_API AHazardSocketMarker : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UBoxComponent* SelectionHitbox;

public:	

	AHazardSocketMarker();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	TObjectPtr<class UHazardSocketComponent> TargetSocket;

protected:

	void SelectionBoxClicked();

	UFUNCTION()
	void OnSelectionBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	UFUNCTION()
	void OnSelectionBoxTouched(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

};