// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GearTypes.generated.h"

#define DEFAULT_PLAYER_NAME "NewPlayer"


class AGearPlaceable;
class AGearPlayerState;

UENUM(BlueprintType)
enum class EGearMatchState : uint8
{
	WaitingForPlayerToJoin,
	AllPlayersJoined,
	SelectingPlaceables,
	Placing,
	Racing_WaitTime,
	Racing,
	PostRace,
	Ended
};

UENUM(BlueprintType)
enum class EPlaceableSocketType : uint8
{
	Road
};

UENUM(BlueprintType)
enum class EPlaceableState : uint8
{
	Preview,
	Selected,
	Idle,
	Enabled
};

USTRUCT(BlueprintType)
struct FPlaceableDescription : public FTableRowBase
{
	GENERATED_BODY()


public:

	UPROPERTY()
	TSubclassOf<AGearPlaceable> Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ClassPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDepracated;

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

USTRUCT(BlueprintType)
struct FCrossTrackProperty
{
	GENERATED_BODY()

public:
	
	FCrossTrackProperty()
		: Position(FVector::Zero())
		, Tangent(FVector::Zero())
		, Up(FVector::Zero())
		, Right(FVector::Zero())
		, FlattenRight(FVector::Zero())
		, Error(0.0f)
	{

	}

	FVector Position;
	FVector Tangent;
	FVector Up;
	FVector Right;
	FVector FlattenRight;
	float Error;
};

USTRUCT(BlueprintType)
struct FCheckpointResult 
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<AGearPlayerState*> PlayerList;

	AGearPlayerState* operator[](int i)
	{
		return PlayerList[i];
	}

	void Add(AGearPlayerState* PlayerState)
	{
		PlayerList.Add(PlayerState);
	}
};