// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GearTypes.h"
#include "GearGameState.generated.h"

class AGearPlayerState;
class ACheckpoint;
class AGearVehicle;
class UPlaceableSocket;
class AGearRoadModule;
class AGearHazard;
class ATrackSpline;
struct FCrossTrackProperty;
class AVehicleCamera;

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
	void Racing_End();
	void Scoreboard_Start();
	void Scoreboard_End();

	float GetEstimatedScoreboardLifespan() const;

	void UpdateFurthestDistanceWithVehicle(AGearVehicle* GearVehicle);

	UFUNCTION()
	void OnRep_GearMatchState(EGearMatchState OldState);

	UFUNCTION()
	void OnRep_RoadModuleStack();

	bool FindStartRoadModuleAndAddToStack();

	bool FindStartCheckpointAndAddToStack();

	UPlaceableSocket* GetRoadStackAttachableSocket();

	ACheckpoint* GetCheckPointAtIndex(int Index);

	void ClearCheckpointResults();

	void RegisterVehicleAtCheckpoint(AGearVehicle* Vehicle, int CheckpointIndex);

	TArray<AGearPlayerState*> GetWinningPlayers() const;

	bool IsAnyPlayerWinning() const;

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

	UPROPERTY(Replicated)
	TArray<FCheckpointResult> CheckpointResults;

	UPROPERTY()
	AVehicleCamera* VehicleCamera;

	UPROPERTY(Replicated, BlueprintReadWrite, VisibleInstanceOnly)
	int32 RoundNumber;

	int LastPlacedCheckpointModuleStackIndex;

	float FurthestReachedDistace;
	int FurthestReachedCheckpoint;

protected:

	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	void AddPlayerState(APlayerState* PlayerState) override;
	void RemovePlayerState(APlayerState* PlayerState) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AVehicleCamera> VehicleCameraClass;
};