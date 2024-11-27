// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GearGameMode.generated.h"

struct FHazardDescription;
class AGearPlayerState;
class AGearHazardActor;
class AHazardPreviewSpawnPoint;

UENUM(BlueprintType)
enum class EGearMatchState : uint8
{
	WaitingForPlayerToJoin,
	AllPlayersJoined,
	SelectingPeices,
	Racing,
	Ended
};

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
	

	UFUNCTION(Server, Reliable)
	void RequestSelectingHazardForPlayer(AGearHazardActor* Hazard, AGearPlayerState* Player);

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
	void StartSelectingPieces();

	bool ShouldAbort();

	void SpawnNewBuilderPawns();
	void SpawnNewHazzards();

	bool LoadHazards();
	bool LoadHazardPreviewSpawnPoints();


	
	EGearMatchState GearMatchState;

	TArray<AGearHazardActor*> PreviewHazards;

	float StartDelayAfterAllJoined = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDataTable* HazardSpawnRulesDataTable;
	
	TArray<FHazardDescription> AvaliableHazards;

	TArray<AHazardPreviewSpawnPoint*> HazardPreviewSpawnPoints;	
};