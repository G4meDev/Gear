// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GearGameMode.generated.h"

UENUM(BlueprintType)
enum class EGearMatchState : uint8
{
	WaitingForPlayerToJoin,
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
	

protected:

	virtual void BeginPlay() override;

	void StartSelectingPieces();

	bool ReadyToStartMatch_Implementation() override;
	void HandleMatchHasStarted() override;

	bool ReadyToEndMatch_Implementation() override;
	void HandleMatchHasEnded() override;

	bool ShouldAbort();
	void HandleMatchAborted() override;

	void StartFirstPhase();

	EGearMatchState GearMatchState;

	float StartDelayAfterAllJoined = 2.0f;
};