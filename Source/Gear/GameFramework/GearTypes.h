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
	Racing,
	Scoreboard,
	GameFinished
};

UENUM(BlueprintType)
enum class EPlaceableState : uint8
{
	None,
	Preview,
	Selected,
	Placing,
	Idle,
	Enabled
};

UENUM(BlueprintType)
enum class EPrebuildState : uint8
{
	Idle = 0,
	Placable,
	NotPlaceable
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

UENUM()
enum class ERoadModuleTraceResult : uint8
{
	NotColliding,
	BodyColliding,
	ExtentColliding
};

UENUM()
enum class EHazardSocketType : uint8
{
	SmallHazard,
	BigHazard,
	SideHazard
};

UENUM(BlueprintType)
enum class EElimanationReason : uint8
{
	Falldown,
	OutsideCameraFrustum
};

UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	None,
	Hammer
};