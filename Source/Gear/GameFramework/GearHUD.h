// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameFramework/Geartypes.h"
#include "GearHUD.generated.h"

class AGearPlayerState;

/**
 * 
 */
UCLASS()
class GEAR_API AGearHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent)
	void ShowWaitingScreen();

	UFUNCTION(BlueprintImplementableEvent)
	void RemoveWaitingScreen();

	UFUNCTION(BlueprintImplementableEvent)
	void AllPlayersJoined();

	UFUNCTION(BlueprintImplementableEvent)
	void StartSelectingPieces(float Time);

	UFUNCTION(BlueprintImplementableEvent)
	void StartPlacingPieces(float Time);

	UFUNCTION(BlueprintImplementableEvent)
	void PlacingPieces_End();

	UFUNCTION(BlueprintImplementableEvent)
	void Racing_Start(float Time);

	UFUNCTION(BlueprintImplementableEvent)
	void Racing_End();

	UFUNCTION(BlueprintImplementableEvent)
	void Scoreboard_Start(float Time, const TArray<FCheckpointResult>& RoundResults);

	UFUNCTION(BlueprintImplementableEvent)
	void Scoreboard_End();

	UFUNCTION(BlueprintImplementableEvent)
	void GameFinished(float Time);

	UFUNCTION(BlueprintImplementableEvent)
	void AddPlayer(AGearPlayerState* InPlayer);

	UFUNCTION(BlueprintImplementableEvent)
	void RemovePlayer(AGearPlayerState* InPlayer);

	UFUNCTION(BlueprintImplementableEvent)
	void NotifyFurthestReachedCheckpoint(int32 FurthestReachedCheckpoint, int32 CheckpointsNum, float FurthestReachedCheckpointTime);
};