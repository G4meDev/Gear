// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GearTypes.h"
#include "GearGameState.generated.h"


/**
 * 
 */
UCLASS()
class GEAR_API AGearGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AGearGameState();

	UFUNCTION()
	void OnRep_GearMatchState();

	UPROPERTY(Replicated)
	double LastStateChangeTime;

	UPROPERTY(ReplicatedUsing=OnRep_GearMatchState)
	EGearMatchState GearMatchState;

protected:
	void AddPlayerState(APlayerState* PlayerState) override;
	void RemovePlayerState(APlayerState* PlayerState) override;


};
