// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VehicleCamera.generated.h"

class AGearGameState;
class UVehicleSpringArm;
class AGearVehicle;
class UCameraComponent;

UCLASS()
class GEAR_API AVehicleCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	AVehicleCamera();

	virtual void Tick(float DeltaTime) override;
	
	void UpdateCamera();

	void UpdateCameraMatrix();

	void MarkTeleport();

	bool IsOutsideCameraFrustum(AActor* Target);

#if WITH_EDITORONLY_DATA
	bool bTestMode = false;
	AGearVehicle* Vehicle;
#endif

protected:
	virtual void BeginPlay() override;

	virtual void BecomeViewTarget(APlayerController* PC) override;

	void UpdateCameraWithTransform(const FTransform& Transform);

	APlayerController* OwnerController;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UVehicleSpringArm* CameraBoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCameraComponent* Camera;

	UPROPERTY()
	AGearGameState* GearGameState;

	FMatrix ViewProjectionMatrix;

	FVector2D ViewportSize;
};