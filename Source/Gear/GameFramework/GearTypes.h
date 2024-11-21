// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GearTypes.generated.h"

#define DEFAULT_PLAYER_NAME "NewPlayer"

USTRUCT(BlueprintType)
struct GEAR_API FGearPersistantData
{
	GENERATED_BODY()

public:
	FGearPersistantData()
		: PlayerName("NewPlayer")
	{  
	
	}

	FString PlayerName;
};
