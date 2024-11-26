// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GearTypes.h"
#include "LobbyGameState.generated.h"

class ALobbyPlayerState;
class ALobbyPlayerController;

/**
 * 
 */
UCLASS()
class GEAR_API ALobbyGameState : public AGameState
{
	GENERATED_BODY()
	

public:

	void RequestColorChangeForPlayer(ALobbyPlayerController* PC, EPlayerColorCode Color);

protected:

	void AddPlayerState(APlayerState* PlayerState) override;

	void RemovePlayerState(APlayerState* PlayerState) override;

private:
	void GetInUseColors(TArray<EPlayerColorCode>& InUseColors);
	void AssignNewColorToPlayer(APlayerState* Player);
};
