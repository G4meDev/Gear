// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VehicleCamera.generated.h"

class AGearGameState;
class UVehicleSpringArm;
class UCameraComponent;

UCLASS()
class GEAR_API AVehicleCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	AVehicleCamera();

	void UpdateCamera();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UVehicleSpringArm* CameraBoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCameraComponent* Camera;

	UPROPERTY()
	AGearGameState* GearGameState;


public:	
	virtual void Tick(float DeltaTime) override;
};