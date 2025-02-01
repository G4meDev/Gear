// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/GearAbility.h"
#include "GearAbility_Hammer.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API AGearAbility_Hammer : public AGearAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* HammerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDecalComponent* AttackRangeDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> AttackEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackHitDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackInnerRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackOuterRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImpulseStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ForceZOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VelocityReductionRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AbilityCooldown;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	int32 RemainingItemUsage;

	UPROPERTY(Replicated)
	float LastActivationTime;

public:
	AGearAbility_Hammer();
	virtual void Destroyed() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnRep_OwningVehicle() override;
	virtual void ActivateAbility() override;

	virtual float GetWidgetValue() override;
	virtual bool CanActivate() override;

protected:
	virtual void OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) override;

	void ShowHammer();

	UFUNCTION(Server, Reliable)
	void Activate_Server();

	UFUNCTION(NetMulticast, Reliable)
	void Activate_Multi();

	void PlayAttackMontage();

	FTimerHandle AttackHitTimerHandle;
	void AttackHit();
};
