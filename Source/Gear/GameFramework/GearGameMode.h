// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GearTypes.h"
#include "GearGameMode.generated.h"

struct FHazardDescription;
class AGearPlayerState;
class AGearPlaceable;
class APlaceableSpawnPoint;

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
	
	void RequestSelectingPlaceableForPlayer(AGearPlaceable* Placeable, AGearPlayerState* Player);

protected:

	virtual void BeginPlay() override;

// ---------------- AGameMode ----------------------------------------------

	bool ReadyToStartMatch_Implementation() override;
	void HandleMatchHasStarted() override;

	bool ReadyToEndMatch_Implementation() override;
	void HandleMatchHasEnded() override;

	void HandleMatchAborted() override;

// -------------------------------------------------------------------------

	bool CheckIsEveryPlayerReady();
	void AllPlayerJoined();

	void StartFirstPhase();
	void StartSelectingPlaceables();
	void AssignPlaceablesToUnowningPlayers();
	FTimerHandle SelectingPiecesTimerHandle;

	bool IsEveryPlayerSelectedPlaceables();
	void StartPlaceing(bool bEveryPlayerIsReady);

	bool ShouldAbort();

	void SpawnNewBuilderPawns();
	void SpawnNewPlaceables();

	bool LoadPlaceables();
	bool LoadPlaceableSpawnPoints();
	
	EGearMatchState GearMatchState;

	TArray<AGearPlaceable*> PreviewPlaceables;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDataTable* PlaceableSpawnRulesDataTable;
	
	TArray<FPlaceableDescription> AvaliablePlaceables;

	TArray<APlaceableSpawnPoint*> HazardPreviewSpawnPoints;
};