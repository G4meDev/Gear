// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Placeable/GearPlaceable.h"
#include "GearRoadModule.generated.h"

class UPlaceableSocket;

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

	void MoveToSocket(UPlaceableSocket* TargetSocket, bool InMirrorX);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UPlaceableSocket* RoadStartSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UPlaceableSocket* RoadEndSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AGearRoadModule> RoadModuleMirroredClass;

	UPlaceableSocket* GetAttachableSocket();

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	bool bMirrorX;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	bool bShouldNotifyGameState;

	bool bGameStateNotified;

protected:
	virtual void BeginPlay() override;

	virtual void PostNetInit() override;
};