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

	void OnTraceStateChanged(ERoadModuleTraceResult Result);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* RoadMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UPlaceableSocket* RoadEndSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USplineComponent* RoadSpline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* MainCollider;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* ExtentCollider;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AGearRoadModule> RoadModuleMirroredClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AGearRoadModule> RoadModuleClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AGearRoadModule> RoadModuleClass_MirrorX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AGearRoadModule> RoadModuleClass_MirrorY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AGearRoadModule> RoadModuleClass_MirrorX_MirrorY;

	UPlaceableSocket* GetAttachableSocket();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RoadLength;

	ERoadModuleTraceResult TraceReult;



#if WITH_EDITORONLY_DATA
	void UpdateSplineParameters();

	void UpdateColliders();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bDirty = false;

	float RotationPivotHeightOffset = -200.0f;
	 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector MainColliderPadding = FVector(-100.0f, 200.0f, 500.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector ExtentColliderSize = FVector(3000.0f, 1000.0f, 500.0f);

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
};