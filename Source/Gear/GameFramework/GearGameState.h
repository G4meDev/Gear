// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GearTypes.h"
#include "GearGameState.generated.h"

class ACheckpoint;
class AGearVehicle;
class UPlaceableSocket;
class AGearRoadModule;
class AGearHazard;
class ATrackSpline;
struct FCrossTrackProperty;

/**
 * 
 */
UCLASS()
class GEAR_API AGearGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	
	AGearGameState();

	void Tick( float DeltaSeconds ) override;

	void AllPlayerJoined_Start();
	void SelectingPlaceables_Start();
	void Placing_Start();
	void Placing_End();
	void Racing_Start();


	UFUNCTION()
	void OnRep_GearMatchState(EGearMatchState OldState);

	UFUNCTION()
	void OnRep_RoadModuleStack();

	bool FindStartRoadModuleAndAddToStack();

	bool FindStartCheckpointAndAddToStack();

	UPlaceableSocket* GetRoadStackAttachableSocket();

	ACheckpoint* GetCheckPointAtIndex(int Index);

	UPROPERTY(ReplicatedUsing=OnRep_GearMatchState)
	EGearMatchState GearMatchState;

	UPROPERTY(Replicated)
	double LastGameStateTransitionTime;

	UPROPERTY()
	ATrackSpline* TrackSpline;

	UPROPERTY(ReplicatedUsing=OnRep_RoadModuleStack, BlueprintReadWrite, VisibleInstanceOnly)
	TArray<AGearRoadModule*> RoadModuleStack;

	UPROPERTY(Replicated, BlueprintReadWrite, VisibleInstanceOnly)
	TArray<ACheckpoint*> CheckpointsStack;

	UPROPERTY(Replicated)
	TArray<AGearVehicle*> Vehicles;

	int LastPlacedCheckpointModuleStackIndex;

	float FurthestReachedDistace;

protected:

	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	void SortPlayersPosition();

	void AddPlayerState(APlayerState* PlayerState) override;
	void RemovePlayerState(APlayerState* PlayerState) override;
};