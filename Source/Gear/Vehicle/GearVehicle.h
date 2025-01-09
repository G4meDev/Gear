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

	float DistanaceAlongTrack;

	UPROPERTY(Replicated)
	int32 TargetCheckpoint;

	class UVehicleAudioComponent* GetVehicleAudioComponent() const { return VehicleAudioComponent; }

	UFUNCTION(BlueprintPure)
	float GetWheelSteerAngle(int32 Index);

	UFUNCTION(BlueprintPure)
	float GetWheelRotation(int32 Index);

	UFUNCTION(BlueprintPure)
	float GetWheelRotationSpeed(int32 Index);

	UFUNCTION(BlueprintPure)
	bool IsWheelOnGround(int32 Index);

	UFUNCTION(BlueprintPure)
	float GetSteerAngle();

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AActor> EliminationFXActorClass;

	void Input_Throttle(const FInputActionInstance& Instance);
	void Input_Brake(const FInputActionInstance& Instance);

	float ThrottleValue;
	float BrakeValue;

	UPROPERTY(ReplicatedUsing=OnRep_GrantedInvincibility)
	bool bGrantedInvincibility;

	float InvincibilityStartTime;

	UPROPERTY(Replicated)
	float SteerAngle;


#if WITH_EDITORONLY_DATA

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AVehicleCamera> VehicleCameraClass;

	bool bInTestMap = false; 
#endif

private:

	UPROPERTY(Category = Vehicle, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UVehicleAudioComponent> VehicleAudioComponent;
};