// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GearTypes.h"
#include "GearAbility.generated.h"

UCLASS()
class GEAR_API AGearAbility : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSlateBrush AbilityPaddingBrush;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FButtonStyle AbilityButtonStyle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EAbilityType AbilityType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimMontage* ItemGrabMontage;

	UPROPERTY(ReplicatedUsing=OnRep_OwningVehicle)
	class AGearVehicle* OwningVehice;

public:	
	AGearAbility();
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void Tick(float DeltaTime) override;

	void SetOwningVehicle(AGearVehicle* InOwningVehice);

	UFUNCTION(BlueprintPure)
	EAbilityType GetAbilityType();

	UFUNCTION()
	virtual void OnRep_OwningVehicle();

	UFUNCTION(BlueprintCallable)
	virtual void ActivateAbility();

	UFUNCTION(BlueprintPure)
	virtual float GetWidgetValue();

protected:
	UFUNCTION()
	virtual void OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UAnimInstance* GetDriverBodyAnimInstance();

	virtual bool CanActivate() const;
};