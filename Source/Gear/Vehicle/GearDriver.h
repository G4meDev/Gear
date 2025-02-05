// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GearTypes.h"
#include "GearDriver.generated.h"

UCLASS()
class GEAR_API AGearDriver : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* BodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ADriverHead* DriverHead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AGearVehicle* OwningVehicle;

	UPROPERTY()
	UMaterialInstanceDynamic* BodyMID;

public:	

	AGearDriver();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	UFUNCTION(BlueprintCallable)
	void ChangeDriverHead(TSubclassOf<class ADriverHead> DriverHeadClass);

	void SetOwningVehicle(AGearVehicle* InOwningVehicle);

	UFUNCTION(BlueprintPure)
	FName GetItemSocketName();

	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetBodyMesh();

	UFUNCTION(BlueprintPure)
	EAbilityType GetAbilityType();

	UFUNCTION(BlueprintCallable)
	void ChangePlayerCustomization(const FPlayerCustomization& PlayerCustomization);

protected:
	void DestroyDriverHead();
};