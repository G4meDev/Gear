// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "GearVehicle.generated.h"

class UChaosWheeledVehicleMovementComponent;
class AGearGameState;
class AVehicleCamera;
class UInputAction;
struct FInputActionInstance;

USTRUCT()
struct FVehicleInput
{
	GENERATED_BODY()

public:
	FVehicleInput(float InThrottle, float InBrake, float InSteer)
		: ThrottleInput(InThrottle)
		, BrakeInput(InBrake)
		, SteerInput(InSteer)
	{ }

	FVehicleInput() : FVehicleInput(0, 0, 0)
	{ }

	void UpdateData(float InThrottle, float InBrake, float InSteer)
	{
		ThrottleInput = InThrottle;
		BrakeInput = InBrake;
		SteerInput = InSteer;
	}

	void Decompress(const FVehicleInputCompressed& CompressedInput);

	float ThrottleInput;
	float BrakeInput;
	float SteerInput;
};

USTRUCT()
struct FVehicleInputCompressed
{
	GENERATED_BODY()

public:
	void Compress(const FVehicleInput& VehicleInput);
	
	uint8 ThrottleInput : 1;
	uint8 BrakeInput : 1;
	uint8 SteerInput : 2;
};



UCLASS()
class GEAR_API AGearVehicle : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:

	AGearVehicle();
	void Tick(float DeltaSeconds) override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	AGearGameState* GetGearGameState();

	bool CanDrive();

	void UpdateDistanceAlongTrack();

	void UpdateStateToVehicle(AGearVehicle* TargetVehicle);

	bool IsOutsideTrack() const;

	UFUNCTION(BlueprintPure)
	UChaosWheeledVehicleMovementComponent* GetChaosMovementComponent();

	UMaterialInstanceDynamic* GetVehicleMaterial();

	void GrantInvincibility();
	void RemoveInvincibility();
	bool HasInvincibility() const;
	bool CanRemoveInvincibility();

	bool IsSpectating();



	UFUNCTION()
	void OnRep_GrantedInvincibility();

	UFUNCTION()
	void OnRep_VehicleInputCompressed();

	float DistanaceAlongTrack;

	UPROPERTY(Replicated)
	int32 TargetCheckpoint;

protected:

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void NotifyControllerChanged() override;

	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;

	virtual void Destroyed() override;

	void UpdateVehicleInputs();

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

	UChaosWheeledVehicleMovementComponent* ChaosMovementComponent;

	UPROPERTY()
	UMaterialInstanceDynamic* VehicleMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* VehicleMaterialParent_Opaque;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* VehicleMaterialParent_Transparent;

	void Input_Throttle(const FInputActionInstance& Instance);
	void Input_Brake(const FInputActionInstance& Instance);

	float ThrottleValue;
	float BrakeValue;

	UFUNCTION(Server, Reliable)
	void SendInputToServer(FVehicleInputCompressed CompressedInput);

	UPROPERTY(ReplicatedUsing=OnRep_GrantedInvincibility)
	bool bGrantedInvincibility;

	float InvincibilityStartTime;

	UPROPERTY(ReplicatedUsing=OnRep_VehicleInputCompressed)
	FVehicleInputCompressed VehicleInputCompressed;

	FVehicleInput VehicleInput;

#if WITH_EDITORONLY_DATA

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AVehicleCamera> VehicleCameraClass;

	bool bInTestMap = false; 
#endif

private:
};