// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GearTypes.h"
#include "GearGameMode.generated.h"

struct FHazardDescription;
class AGearPlayerController;
class AGearRoadModule;
class UPlaceableSocket;
class AGearBuilderPawn;
class AGearGameState;
class AGearPlayerState;
class AGearPlaceable;
class APlaceableSpawnPoint;
class AGearVehicle;
class ACheckpoint;

/**
 * 
 */
UCLASS()
class GEAR_API AGearGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AGearGameMode();

	void Tick(float DeltaSeconds) override;

	void RacingTick(float DeltaSeconds);
	
	void RequestSelectingPlaceableForPlayer(AGearPlaceable* Placeable, AGearBuilderPawn* Player);

	void RequestPlaceRoadModuleForPlayer(AGearPlayerController* PC, TSubclassOf<AGearRoadModule> RoadModule, UPlaceableSocket* TargetSocket, bool bMirrorX);

	void VehicleReachedCheckpoint(AGearVehicle* Vehicle, ACheckpoint* TargetCheckpoint);

protected:

	virtual void BeginPlay() override;

	AGearGameState* GearGameState;

// ---------------- AGameMode ----------------------------------------------

	bool ReadyToStartMatch_Implementation() override;
	void HandleMatchHasStarted() override;

	bool ReadyToEndMatch_Implementation() override;
	void HandleMatchHasEnded() override;

	void HandleMatchAborted() override;

// -------------------------------------------------------------------------

	void SetGearMatchState(EGearMatchState InGearMatchState);

	bool CheckIsEveryPlayerReady();
	void AllPlayerJoined();
	void StartNewRound();

	void StartSelectingPlaceables();
	void AssignPlaceablesToUnowningPlayers();
	FTimerHandle SelectingPiecesTimerHandle;

	bool IsEveryPlayerSelectedPlaceables();
	void StartPlaceing(bool bEveryPlayerIsReady);
	FTimerHandle PlacingTimerHandle;

	bool IsEveryPlayerPlaced();
	void PlaceUnplaced();
	void DestroyActors();
	void SpawnActors();
	void DestroyPawns();
	void StartRacingAtCheckpoint(int CheckpointIndex, AGearVehicle* InstgatorVehicle);
	void StartRacing(bool bEveryPlayerPlaced);

	AGearRoadModule* AddRoadModule(TSubclassOf<AGearRoadModule> RoadModule, bool bMirrorX);
	bool ShouldAddCheckpoint() const;
	void AddCheckpoint();

	void StartScoreboard();
	void ScoreboardLifespanFinished();
	FTimerHandle ScoreboardTimerHandle;

	void GameFinished();

	bool ShouldAbort();

	void SpawnNewBuilderPawns();
	void SpawnNewPlaceables();

	bool LoadPlaceables();
	bool LoadPlaceableSpawnPoints();
	
	// --------------------------------------------------------------------------

	bool ShouldVehicleDie(AGearVehicle* Vehicle) const;
	void DestroyVehicle(AGearVehicle* Vehicle);

	EGearMatchState GearMatchState;

	TArray<AGearPlaceable*> PreviewPlaceables;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDataTable* PlaceableSpawnRulesDataTable;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AGearRoadModule> CheckpointModuleClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ACheckpoint> CheckpointClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CheckpointDistance;
	
	TArray<FPlaceableDescription> AvaliablePlaceables;
	TArray<APlaceableSpawnPoint*> HazardPreviewSpawnPoints;
};