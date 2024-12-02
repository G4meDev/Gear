// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GearTypes.h"
#include "GearGameState.generated.h"

class UPlaceableSocket;
class AGearRoadModule;
class AGearHazard;

/**
 * 
 */
UCLASS()
class GEAR_API AGearGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	
	AGearGameState();

	void AllPlayerJoined_Start();
	void SelectingPlaceables_Start();
	void Placing_Start();


	UFUNCTION()
	void OnRep_GearMatchState(EGearMatchState OldState);

	UFUNCTION()
	void OnRep_RoadModuleStack();

	bool FindStartRoadModuleAndAddToStack();

	UPlaceableSocket* GetRoadEndSocket();

	UPROPERTY(ReplicatedUsing=OnRep_GearMatchState)
	EGearMatchState GearMatchState;

	UPROPERTY(Replicated)
	double LastGameStateTransitionTime;

	UPROPERTY(ReplicatedUsing=OnRep_RoadModuleStack)
	TArray<AGearRoadModule*> RoadModuleStack;

protected:

	virtual void BeginPlay() override;

	void AddPlayerState(APlayerState* PlayerState) override;
	void RemovePlayerState(APlayerState* PlayerState) override;
};