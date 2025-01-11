// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Placeable/GearPlaceable.h"
#include "GearHazard.generated.h"


UCLASS(Blueprintable)
class GEAR_API AGearHazard : public AGearPlaceable
{
	GENERATED_BODY()
	
public:
	AGearHazard();

	void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;
	
	void SetPreview() override;
	void SetSelectedBy(AGearBuilderPawn* Player) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 PlacingCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<class AHazardSocketMarker> SocketMarkerClass;

protected:
	virtual void BeginPlay() override;


	#if WITH_EDITORONLY_DATA

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bDirty = false;

#endif

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif

// ---------------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintImplementableEvent)
	void Idle_Start();

	UFUNCTION(BlueprintImplementableEvent)
	void Idle_End();

	UFUNCTION(BlueprintImplementableEvent)
	void Enabled_Start();

	UFUNCTION(BlueprintImplementableEvent)
	void Enabled_End();

	virtual void OnIdle_Start() override;
	virtual void OnIdle_End() override;

	virtual void OnEnabled_Start() override;
	virtual void OnEnabled_End() override;

};
