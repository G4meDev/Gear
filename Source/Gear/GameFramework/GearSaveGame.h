// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameFramework/GearTypes.h"
#include "GearSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UGearSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UGearSaveGame() 
		: PlayerName(DEFAULT_PLAYER_NAME)
	{

	}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	FString PlayerName;
};
