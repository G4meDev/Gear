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
	
	UFUNCTION(BlueprintPure)
	bool IsInvincible();

	void RemoveInvincibility();
	bool HasInvincibility() const;
	bool CanRemoveInvincibility();

	UFUNCTION(BlueprintPure)
	bool IsSpectating();

	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetDriverBody();

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

// 	UFUNCTION(BlueprintPure)
// 	const TWeakObjectPtr<UPhysicalMaterial> GetWheelContactPhysicMaterial(int32 Index);

	UFUNCTION(BlueprintPure)
	bool IsWheelSkiding(int32 Index);

	UFUNCTION(BlueprintPure)
	float GetSteerAngle();

	UFUNCTION(BlueprintPure)
	bool HasAbility();

	UFUNCTION(BlueprintPure)
	AGearAbility* GetAbility();

	UFUNCTION()
	void OnRep_Ability();

	UFUNCTION(BlueprintCallable)
	void GrantAbility(TSubclassOf<class AGearAbility> AbilityClass);

	UFUNCTION(BlueprintCallable)
	void ClearAbility();

	UFUNCTION(BlueprintPure)
	AGearDriver* GetDriver();

	UFUNCTION(BlueprintPure)
	int32 GetNumWheelsOnGround();

	UFUNCTION(BlueprintPure)
	bool IsOnGround();

	UFUNCTION(BlueprintCallable)
	void ReduceVelocityBeRatio(float Ratio);

	void UpdateControl();

	void UpdateWheelEffect(float DeltaTime);

protected:

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void PostInitializeComponents() override;

	virtual void NotifyControllerChanged() override;

	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;

	virtual void Destroyed() override;

	UFUNCTION()
	void OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);

	void UpdateVehicleInputs();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* SteerActionInput;

	void Input_Steer(const FInputActionInstance& Instance);

	float SteerValue;

	AGearGameState* GearGameState;

	APlayerController* AuthorativeController;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* ThrottleActionInput;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* BrakeActionInput;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* AbilityActionInput;

	UChaosWheeledVehicleMovementComponent* ChaosMovementComponent;

	UPROPERTY()
	UMaterialInstanceDynamic* VehicleMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* VehicleMaterialParent_Opaque;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* VehicleMaterialParent_Transparent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AActor> EliminationFXActorClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AGearDriver> DriverClass;

	AGearDriver* Driver;

	void Input_Throttle(const FInputActionInstance& Instance);
	void Input_Brake(const FInputActionInstance& Instance);

	void Input_Ability(const FInputActionInstance& Instance);

	float ThrottleValue;
	float BrakeValue;

	UPROPERTY(ReplicatedUsing=OnRep_GrantedInvincibility)
	bool bGrantedInvincibility;

	float InvincibilityStartTime;

	UPROPERTY(Replicated)
	float SteerAngle;

	UPROPERTY(ReplicatedUsing=OnRep_Ability)
	class AGearAbility* Ability;

#if WITH_EDITORONLY_DATA

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AVehicleCamera> VehicleCameraClass;

	bool bInTestMap = false; 

#endif

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Control)
	float AirTorqueControl;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Control)
	bool bDrawDebugs = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Control)
	FVector InputDirection;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Control)
	float CurrentSpeed;

// ---------------------------------------------------------------------------------------------

	void SpawnNewWheelEffect(int WheelIndex);

	UPROPERTY(Transient)
	class UNiagaraComponent* DustPSC[3];

	UPROPERTY(EditDefaultsOnly, Category="Effect")
	class UWheelEffectType* WheelEffect;

	UPROPERTY(EditDefaultsOnly, Category="Effect")
	float WheelEffectOffset;

private:

	UPROPERTY(Category = Vehicle, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UVehicleAudioComponent> VehicleAudioComponent;

};