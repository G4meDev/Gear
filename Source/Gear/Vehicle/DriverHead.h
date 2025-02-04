// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DriverHead.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOwningDriverChanged);

UCLASS()
class GEAR_API ADriverHead : public AActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMeshComponent* HeadMesh;

	UPROPERTY()
	class AGearDriver* OwningDriver;

	void SetOwningDriver(class AGearDriver* InOwningDriver);

	UPROPERTY(BlueprintAssignable)
	FOnOwningDriverChanged OnOwningDriverChanged;

	friend class AGearDriver;

public:	

	ADriverHead();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	class AGearDriver* GetOwningDriver();
};