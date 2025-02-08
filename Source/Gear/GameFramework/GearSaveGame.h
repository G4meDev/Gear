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
		, SoundAmplitude(0.5f)
		, MusicAmplitude(0.5f)
		, QualityLevel(2)
		, Language(ELanguageOption::Farsi)
	{

	}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	float SoundAmplitude;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	float MusicAmplitude;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	int32 QualityLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
	ELanguageOption Language;
};