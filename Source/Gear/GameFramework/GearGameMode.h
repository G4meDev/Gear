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

	void RequestPlaceRoadModuleForPlayer(AGearPlayerController* PC, TSubclassOf<AGearRoadModule> RoadModule, bool bMirrorX);

	void RequestPlaceHazardForPlayer(AGearPlayerController* PC, class UHazardSocketComponent* TargetSocket);

	void VehicleReachedCheckpoint(AGearVehicle* Vehicle, ACheckpoint* TargetCheckpoint);

protected:

	virtual void BeginPlay() override;

	AGearGameState* GearGameState;

	void HandleMatchAborted() override;

	void SetGearMatchState(EGearMatchState InGearMatchState);

	bool CheckIsEveryPlayerReady();
	void AllPlayerJoined();
	void StartNewRound();

	void StartSelectingPlaceables();
	void AssignPlaceablesToUnowningPlayers();
	FTimerHandle SelectingPiecesTimerHandle;

	bool IsEveryPlayerSelectedPlaceables();
	void StartPlaceing(bool bEveryPlayerIsReady);
	void EndPlaceing();
	FTimerHandle PlacingTimerHandle;

	bool IsEveryPlayerPlaced();
	void DestroyActors();
	void SpawnActors();
	void DestroyPawns();
	void SpawnVehicleAtCheckpoint(AGearPlayerState* Player, ACheckpoint* Checkpoint, bool GrantInvincibility);
	void StartRacingAtCheckpoint(ACheckpoint* Checkpoint, AGearVehicle* InstgatorVehicle);
	void StartRacing();
	FTimerHandle RacingTimerHandle;
	FTimerHandle RacingEndDelayTimerHandle;

	AGearRoadModule* AddRoadModule(TSubclassOf<AGearRoadModule> RoadModule);
	class AGearHazard* AddHazard(TSubclassOf<AGearPlaceable> HazardClass, UHazardSocketComponent* TargetSocket);
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

	bool IsEveryPlayerEliminated() const;
	bool ShouldVehicleDie(AGearVehicle* Vehicle, EElimanationReason& EliminationReson);
	void DestroyVehicle(AGearVehicle* Vehicle);
	void DestroyAllVehicles(bool bIncludeSpectators = false);

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