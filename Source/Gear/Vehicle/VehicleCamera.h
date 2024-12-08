// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VehicleCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GEAR_API AVehicleCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	AVehicleCamera();

	void MarkTeleport();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCameraComponent* Camera;

public:	
	virtual void Tick(float DeltaTime) override;

	void ClearTeleport();
};
