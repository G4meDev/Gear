
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystems/RoundResetable.h"
#include "GameFramework/GearTypes.h"
#include "GearPlaceable.generated.h"

class AGearBuilderPawn;
class UBoxComponent;
class UStaticMeshComponent;
class APlaceableSpawnPoint;

UCLASS(Blueprintable)
class GEAR_API AGearPlaceable : public AActor, public IRoundResetable
{
	GENERATED_BODY()
	
public:	
	AGearPlaceable();

	void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	void MarkNotReplicated();

	UFUNCTION(BlueprintCallable)
	virtual void RoundReset() override;

	virtual void SetPreview();

	virtual void SetSelectedBy(AGearBuilderPawn* Player);
	
	virtual void  SetPlacing();

	virtual void SetIdle();

	virtual void SetEnabled();

	UFUNCTION(BlueprintImplementableEvent)
	void RoundRestBlueprintEvent();

	UPROPERTY(Replicated)
	AGearPlayerState* OwningPlayer;

	bool HasOwningPlayer() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* ModulesStack;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* MainModules;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* PrebuildModules;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* SelectionHitbox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* PreviewRotationPivot;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PreviewScale;

	void AttachToSpawnPoint(APlaceableSpawnPoint* SpawnPoint);

	UFUNCTION()
	void OnRep_PlaceableState(EPlaceableState OldState);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_PlaceableState, BlueprintReadWrite, EditAnywhere)
	EPlaceableState PlaceableState;

	void SelectionBoxClicked();

	UFUNCTION()
	void OnSelectionBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	UFUNCTION()
	void OnSelectionBoxTouched(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> PrebuildMaterials;

//----------------------------------------------------------------------------

	virtual void OnPreview_Start();
	virtual void OnPreview_End();

	virtual void OnSelected_Start();
	virtual void OnSelected_End();

	virtual void OnPlacing_Start();
	virtual void OnPlacing_End();

	virtual void OnIdle_Start();
	virtual void OnIdle_End();

	virtual void OnEnabled_Start();
	virtual void OnEnabled_End();

	void SetSelectionBoxEnabled(bool bEnabled);

	void SetPrebuildState(EPrebuildState State);
};