// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LobbyGameState.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API ALobbyGameState : public AGameState
{
	GENERATED_BODY()
	
protected:
	void AddPlayerState(APlayerState* PlayerState) override;

	void RemovePlayerState(APlayerState* PlayerState) override;
};
