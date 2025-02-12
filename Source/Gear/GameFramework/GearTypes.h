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
	None,
	WaitingForPlayerToJoin,
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

USTRUCT(BlueprintType)
struct FRoundResult 
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FCheckpointResult> CheckpointArray;

	FCheckpointResult& operator[](int i)
	{
		return CheckpointArray[i];
	}

	void Add(const FCheckpointResult& CheckpointResult)
	{
		CheckpointArray.Add(CheckpointResult);
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

UENUM(BlueprintType)
enum class EWidgetOrder : uint8
{
	LowPriority = 0,
	PauseMenu = 1
};

USTRUCT(BlueprintType)
struct FCustomizationColor
{
	GENERATED_BODY()

public:
	
	FCustomizationColor()
	{
		ColorCode = EPlayerColorCode::Black;
		ResolveColorCode();
	};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPlayerColorCode ColorCode;

	UPROPERTY(BlueprintReadOnly, NotReplicated)
	FColor Color;

	UPROPERTY(BlueprintReadOnly, NotReplicated)
	FLinearColor LinearColor;

	void ResolveColorCode();
};

UENUM(BlueprintType)
enum class EDriverHead: uint8
{
	Box,
	Max
};

USTRUCT(BlueprintType)
struct FCustomizationHead
{
	GENERATED_BODY()

public:
	FCustomizationHead()
	{
		Type = EDriverHead::Box;
		ResolveClass();
	};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDriverHead Type;

	UPROPERTY(BlueprintReadOnly, NotReplicated)
	UClass* Class;

	void ResolveClass();
};

USTRUCT(BlueprintType)
struct FPlayerCustomization
{
	GENERATED_BODY()

public:
	FPlayerCustomization()
	{

	};

	void ResolveData()
	{
		HeadType.ResolveClass();

		TricycleColor.ResolveColorCode();
		ClothColor.ResolveColorCode();
		PantColor.ResolveColorCode();
		HandColor.ResolveColorCode();
		ShoeColor.ResolveColorCode();
	}

	static FPlayerCustomization GetRandomCustomization();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCustomizationHead HeadType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCustomizationColor TricycleColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCustomizationColor ClothColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCustomizationColor PantColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCustomizationColor HandColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCustomizationColor ShoeColor;
};

UENUM(BlueprintType)
enum class EPlayerDisconnectionReason : uint8
{
	ConnectionFailure,
	Kicked,
	ServerTravel,
	Quit,
	FullLobby,
	WrongPassword,
	TryingToStartGame
};

USTRUCT()
struct FPlayerDisconnectionStrings
{
	GENERATED_BODY()

public:
	static FString FullLobbyString;
	static FString WrongPasswordString;
	static FString TryingToStartGame;
};

UENUM(BlueprintType)
enum class ELanguageOption : uint8
{
	Farsi = 0,
	English,
	Max
};

UENUM(BlueprintType)
enum class ELobbyGameState : uint8
{
	WaitingForPlayers,
	StartingGame
};