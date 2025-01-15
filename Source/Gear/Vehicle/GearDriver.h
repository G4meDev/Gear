// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GearDriver.generated.h"

UCLASS()
class GEAR_API AGearDriver : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* BodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* HeadMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AGearVehicle* OwningVehicle;

public:	

	AGearDriver();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void SetOwningVehicle(AGearVehicle* InOwningVehicle);

	UFUNCTION(BlueprintPure)
	FName GetItemSocketName();

	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetBodyMesh();
	
	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetHeadMesh();
};
