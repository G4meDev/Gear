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
	void AllPlayerJoined_End();
	void SelectingPlaceables_Start();
	void Placing_Start();
	void Placing_End();
	void Racing_Start();
	void Racing_End();
	void Scoreboard_Start();
	void Scoreboard_End();
	void GameFinished();

	float GetEstimatedScoreboardLifespan() const;

	void UpdateFurthestDistanceWithVehicle(AGearVehicle* GearVehicle);

	UFUNCTION()
	void OnRep_GearMatchState(EGearMatchState OldState);

	void OnModuleStackChanged();

	UFUNCTION()
	void OnRep_RoadModuleSocketTransform();

	bool FindStartRoadModuleAndAddToStack();

	bool FindStartCheckpointAndAddToStack();

	UPlaceableSocket* GetRoadStackAttachableSocket();

	ACheckpoint* GetCheckPointAtIndex(int Index);

	ACheckpoint* GetFurthestReachedCheckpoint() const;
	ACheckpoint* GetNextFurthestReachedCheckpoint() const;

	void UpdateRoadModuleSocket();


	void ClearOccupiedVehicleStarts();
	void ClearCheckpointResults();

	void RegisterVehicleAtCheckpoint(AGearVehicle* Vehicle, ACheckpoint* Checkpoint);

	TArray<AGearPlayerState*> GetWinningPlayers() const;
	bool IsAnyPlayerWinning() const;

	UFUNCTION(BlueprintPure)
	TArray<AGearPlayerState*> GetPlayersPlacement();


	UFUNCTION(BlueprintPure)
	bool IsCountDown();

	UPROPERTY(ReplicatedUsing=OnRep_GearMatchState)
	EGearMatchState GearMatchState;

	UPROPERTY(Replicated)
	double LastGameStateTransitionTime;

	UPROPERTY()
	ATrackSpline* TrackSpline;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
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

	UPROPERTY(Replicated, BlueprintReadWrite, VisibleInstanceOnly)
	float LastCountDownTime;

	float LastCheckpointStartTime;

	int LastPlacedCheckpointModuleStackIndex;

	float FurthestReachedDistace;

	UPROPERTY(ReplicatedUsing=OnRep_FurthestReachedCheckpoint, BlueprintReadWrite)
	int FurthestReachedCheckpoint;

	UPROPERTY(ReplicatedUsing=OnRep_RoadModuleSocketTransform)
	FTransform RoadModuleSocketTransform;

//---------------------------------------------------------------------------------------------------------------------------

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastSelectedEvent_Multi(AGearPlayerState* PlayerState, TSubclassOf<AGearPlaceable> PlaceableClass);

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastPlacedEvent_Multi(AGearPlayerState* PlayerState, TSubclassOf<AGearPlaceable> PlaceableClass);

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastEliminationEvent_Multi(AGearPlayerState* PlayerState, EElimanationReason ElimanationReason);

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastReachedCheckpointEvent_Multi(AGearPlayerState* PlayerState, ACheckpoint* Checkpoint, int32 Position);

	UFUNCTION()
	void OnRep_FurthestReachedCheckpoint();

protected:

	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	void AddPlayerState(APlayerState* PlayerState) override;
	void RemovePlayerState(APlayerState* PlayerState) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AVehicleCamera> VehicleCameraClass;
	
//----------------------------------------------------------------------------------------------------------------------------

 	UPROPERTY(BlueprintReadWrite, EditAnywhere)
 	TSubclassOf<UUserWidget> NotifictionBoardClass;

	UPROPERTY()
	class UNotifictionBoardWidget* NotifictionBoardWidget;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* SelectedSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* PlacedSound;
};