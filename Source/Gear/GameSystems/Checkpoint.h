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
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	TArray<UVehicleStart*> StartPoints;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	int CheckpointIndex;
	
	UPROPERTY(ReplicatedUsing=OnRep_LastStartTime, BlueprintReadWrite, EditAnywhere)
	float LastStartTime;

	UFUNCTION()
	void OnRep_LastStartTime();

	UVehicleStart* GetVehicleStart();

	void ClearOccupied();

	UFUNCTION(NetMulticast, Reliable)
	void PlayerReachedCheckpoint_Multi(class AGearPlayerState* Player, int32 Position);

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
	USkeletalMeshComponent* StartlineMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* LapHitbox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInstancedStaticMeshComponent* StartPositionMeshes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UNiagaraComponent* CheerParticleComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UVehicleStart* StartPonit_1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UVehicleStart* StartPonit_2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UVehicleStart* StartPonit_3;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UVehicleStart* StartPonit_4;

// -----------------------------------------------------------------

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UArrowComponent* StartPoint_1_Arrow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UArrowComponent* StartPoint_2_Arrow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UArrowComponent* StartPoint_3_Arrow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UArrowComponent* StartPoint_4_Arrow;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* CheerSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* CountDownSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimationAsset* CloseAnimation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimationAsset* OpenAnimation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAnimationAsset* OpeningAnimation;

	UPROPERTY()
	UMaterialInstanceDynamic* StartlineMaterial;

	void SetLightIndex(int Index);

	void SetState(int State);
};
