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

	virtual void PostNetInit() override;

	void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintPure)
	class AGearPlayerController* GetLocalPlayer();


	void Waiting_Start();
	void Waiting_End();
	void Selecting_Start();
	void Selecting_End();
	void Placing_Start();
	void Placing_End();
	void Racing_Start();
	void Racing_End();
	void Scoreboard_Start();
	void Scoreboard_End();
	void Finishboard_Start();
	void Finishboard_End();

	void NotifyAllPlayersJoined();

	float GetEstimatedScoreboardLifespan() const;

	void UpdateFurthestDistanceWithVehicle(AGearVehicle* GearVehicle);

	UFUNCTION()
	void OnRep_GearMatchState(EGearMatchState OldState);

	void OnModuleStackChanged();

	UFUNCTION()
	void OnRep_RoadModuleSocketTransform();

	bool FindStartRoadModuleAndAddToStack();

	bool FindStartCheckpointAndAddToStack();

	USceneComponent* GetRoadStackAttachableSocket();

	ACheckpoint* GetCheckPointAtIndex(int Index);

	ACheckpoint* GetFurthestReachedCheckpoint() const;
	ACheckpoint* GetNextFurthestReachedCheckpoint() const;

	void UpdateRoadModuleSocket();

	void MarkActorsIdle();
	void MarkActorsEnabled();

	void ClearOccupiedVehicleStarts();
	void ClearCheckpointResults();

	void RegisterVehicleAtCheckpoint(AGearVehicle* Vehicle, ACheckpoint* Checkpoint);

	TArray<AGearPlayerState*> GetWinningPlayers() const;
	bool IsAnyPlayerWinning() const;

	UFUNCTION(BlueprintPure)
	TArray<AGearPlayerState*> GetPlayersPlacement();

	void UpdateFurthestReachedCheckpoint(int32 CheckpointIndex);

	UFUNCTION(BlueprintPure)
	bool IsCountDown();

	float TimeFromLastTransition() const;

	void GetWorldBounds(FVector& World_Min, FVector& World_Max);

	UFUNCTION(BlueprintPure)
	int32 GetSpectatingPlayerCount();

	UFUNCTION(BlueprintPure)
	int32 GetDrivingPlayerCount();

	UPROPERTY(ReplicatedUsing=OnRep_GearMatchState)
	EGearMatchState GearMatchState;

	UPROPERTY(Replicated, BlueprintReadOnly)
	double LastGameStateTransitionTime;

	UPROPERTY()
	ATrackSpline* TrackSpline;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
	TArray<AGearRoadModule*> RoadModuleStack;

	UPROPERTY(Replicated, BlueprintReadWrite, VisibleInstanceOnly)
	TArray<ACheckpoint*> CheckpointsStack;

	UPROPERTY(Replicated)
	TArray<AGearVehicle*> Vehicles;

	UPROPERTY()
	FRoundResult OngoingRoundResult;

	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<FRoundResult> RoundsResult;

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
	int32 FurthestReachedCheckpoint;

	UPROPERTY(Replicated)
	float FurthestReachedCheckpointTime;

	UPROPERTY(ReplicatedUsing=OnRep_RoadModuleSocketTransform, BlueprintReadOnly)
	FTransform RoadModuleSocketTransform;

	float ServerTimeDelay;

	void SetServerTimeDelay(float InServerTimeDelay);

	UFUNCTION(BlueprintPure)
	float GetServerWorldTime();

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastSelectedEvent_Multi(AGearPlayerState* PlayerState, TSubclassOf<AGearPlaceable> PlaceableClass, AGearPlaceable* Placeable);

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastPlacedEvent_Multi(AGearPlayerState* PlayerState, TSubclassOf<AGearPlaceable> PlaceableClass);

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastEliminationEvent_Multi(AGearPlayerState* PlayerState, EElimanationReason ElimanationReason, float EliminationTime, int32 RemainingPlayersCount);

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastReachedCheckpointEvent_Multi(AGearPlayerState* PlayerState, ACheckpoint* Checkpoint, int32 Position, int32 AllCheckpointNum, float ReachTime);
	
	UFUNCTION(NetMulticast, Reliable)
	void BroadcastRaceStartEvent_Multi(float StartTime, bool bWithCountDown);

	UFUNCTION()
	void OnRep_FurthestReachedCheckpoint();

protected:

	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	void AddPlayerState(APlayerState* PlayerState) override;
	void RemovePlayerState(APlayerState* PlayerState) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AVehicleCamera> VehicleCameraClass;

	UPROPERTY()
	class AGearPlayerController* LocalPlayer;
	
	UFUNCTION()
	void OnRep_EveryPlayerReady();

	UPROPERTY(ReplicatedUsing=OnRep_EveryPlayerReady)
	bool bEveryPlayerReady;

//----------------------------------------------------------------------------------------------------------------------------

	UPROPERTY(Replicated)
	FVector WorldMin;
	UPROPERTY(Replicated)
	FVector WorldMax;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* SelectedSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UNiagaraSystem* SelectedFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundBase* PlacedSound;

	friend class AGearGameMode;
};