// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

class AGearGameState;
class UVehicleStart;
class UBoxComponent;
class UArrowComponent;

UCLASS()
class GEAR_API ACheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ACheckpoint();

	void Init(float InWidth, float InHeight, float InLength, float InLateralSeperationRatio, float InLongitudinalSeperation);

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Width;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Height;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Length;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float LateralSeperationRatio;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float LongitudinalSeperation;

	UPROPERTY()
	TArray<UVehicleStart*> StartPoints;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int CheckpointIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float LastStartTime;

	UFUNCTION(NetMulticast, Reliable)
	void StartRace_RPC(float StartTime);

protected:

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void LapHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* LapHitbox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* StartLineMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInstancedStaticMeshComponent* StartPositionMeshes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMeshComponent* HandMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UVehicleStart* StartPonit_1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UVehicleStart* StartPonit_2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UVehicleStart* StartPonit_3;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UVehicleStart* StartPonit_4;

#if WITH_EDITORONLY_DATA

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UArrowComponent* StartPoint_1_Arrow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UArrowComponent* StartPoint_2_Arrow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UArrowComponent* StartPoint_3_Arrow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UArrowComponent* StartPoint_4_Arrow;

#endif
};
