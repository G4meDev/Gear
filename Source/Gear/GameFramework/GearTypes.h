// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GearTypes.generated.h"

#define DEFAULT_PLAYER_NAME "NewPlayer"


class AGearHazardActor;

UENUM(BlueprintType)
enum class EGearMatchState : uint8
{
	WaitingForPlayerToJoin,
	AllPlayersJoined,
	SelectingPeices,
	PlacingPieces,
	Racing,
	Ended
};

USTRUCT(BlueprintType)
struct FHazardDescription : public FTableRowBase
{
	GENERATED_BODY()


public:

	UPROPERTY()
	TSubclassOf<AGearHazardActor> Class;
	
	UPROPERTY()
	bool HasBeenSpawned = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ClassPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool OnlySpawnOne = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnChance_Round_1 = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnChance_Round_2 = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnChance_Round_3 = 10.0f;
};


USTRUCT(BlueprintType)
struct GEAR_API FGearHostInfo
{
	GENERATED_BODY()

public:
	FGearHostInfo(const FString& InName, const FString& InIP)
		: Name(InName)
		, IP(InIP)
	{

	}

	FGearHostInfo() : FGearHostInfo("Host", "IP")
	{

	}

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FString IP;
};

UENUM(BlueprintType)
enum class EPlayerColorCode : uint8 
{
	Black,
	NileBlue,
	PansyPurple,
	ElfGreen,
	BrownRust,
	Wenge,
	Cloud,
	Linen,
	RedRibbon,
	Squash,
	BananaYellow,
	GreenSnake,
	PictonBlue,
	GrayishPurple,
	DeepBlush,
	DeepPeach,
	MAX_COLOR
};