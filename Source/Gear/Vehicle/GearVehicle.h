// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "GearVehicle.generated.h"

class AGearGameState;
class AVehicleCamera;
class UInputAction;
struct FInputActionInstance;

/**
 * 
 */
UCLASS()
class GEAR_API AGearVehicle : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:

	AGearVehicle();
	void Tick(float DeltaSeconds) override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool CanDrive();

	void UpdateDistanceAlongTrack();

	float DistanaceAlongTrack;

	int TargetCheckpoint;

protected:

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void NotifyControllerChanged() override;

	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;

	virtual void Destroyed() override;

	bool IsOutsideTrack() const;

	void Killed();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* SteerActionInput;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UUserWidget> VehicleInputWidgetClass;

	UPROPERTY()
	UUserWidget* VehicleInputWidget;

	void Input_Steer(const FInputActionInstance& Instance);

	float SteerValue;

	AGearGameState* GearGameState;

	APlayerController* AuthorativeController;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* ThrottleActionInput;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* BrakeActionInput;

	void Input_Throttle(const FInputActionInstance& Instance);
	void Input_Brake(const FInputActionInstance& Instance);

	float ThrottleValue;
	float BrakeValue;

#if WITH_EDITORONLY_DATA

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AVehicleCamera> VehicleCameraClass;

	bool bInTestMap = false; 
#endif

private:
};