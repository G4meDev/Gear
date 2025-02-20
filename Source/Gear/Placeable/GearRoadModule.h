// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Placeable/GearPlaceable.h"
#include "GameFramework/GearTypes.h"
#include "GearRoadModule.generated.h"

class UPlaceableSocket;
class USplineComponent;

/**
 * 
 */
UCLASS()
class GEAR_API AGearRoadModule : public AGearPlaceable
{
	GENERATED_BODY()
	
public:
	AGearRoadModule();

	void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	void SetPreview() override;
	void SetSelectedBy(AGearBuilderPawn* Player) override;

	void MoveToSocketTransform(const FTransform& TargetSocket);

	void OnTraceStateChanged();

	void InitializePrebuildMaterials();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* RoadMesh;

 	UPROPERTY(BlueprintReadWrite, EditAnywhere)
 	USceneComponent* RoadEndSocketComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* HazardSockets;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* SpawnableSockets;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* ExtentCollider;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USplineComponent* RoadSpline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)	
	UBoxComponent* Collider_1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)	
	UBoxComponent* Collider_2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)	
	UBoxComponent* Collider_3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)	
	UBoxComponent* Collider_4;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)	
	UBoxComponent* Collider_5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)	
	UBoxComponent* Collider_6;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)	
	UBoxComponent* Collider_7;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)	
	UBoxComponent* Collider_8;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* MainColliders;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AGearRoadModule> RoadModuleClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AGearRoadModule> RoadModuleClass_MirrorX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AGearRoadModule> RoadModuleClass_MirrorY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AGearRoadModule> RoadModuleClass_MirrorX_MirrorY;

	USceneComponent* GetAttachableSocket();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RoadLength;

	UPROPERTY(BlueprintReadOnly)
	bool bPlacingModuleInCollision;

	UPROPERTY()
 	TArray<UMaterialInstanceDynamic*> PrebuildMaterials;

	void SetPrebuildState(EPrebuildState State);

	bool bActivePlacingModule;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FCollisionBox> PlacingColliderBoxs;

	UPROPERTY(Replicated)
	bool bShouldPlayBuildAnim;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayBuildAnim();


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bMirrorX = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bMirrorY = false;

#if WITH_EDITORONLY_DATA
	void UpdateSplineParameters();

	void UpdateColliders();

	void UpdateSplineFromParent(bool InMirrorX, bool InMirrorY);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bDirty = false; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bSplineDirty = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform CollidersRefreneTransform = FTransform::Identity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform ExtendCollidersRefreneTransform = FTransform::Identity;

	float RotationPivotHeightOffset = -200.0f;
	 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector MainColliderPadding = FVector(-100.0f, 200.0f, 500.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector ExtentColliderSize = FVector(3000.0f, 1000.0f, 500.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SelectionBoxPadding = 200.0f;

#endif

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void PostNetInit() override;

	// -----------------------------------------------------------------------------

	void SetMainColliderEnabled(bool bEnabled);

	virtual void OnIdle_Start() override;
	virtual void OnIdle_End() override;

	virtual void OnEnabled_Start() override;
	virtual void OnEnabled_End() override;

	friend class AGearBuilderPawn;
};