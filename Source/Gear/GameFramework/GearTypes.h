// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GearTypes.generated.h"

#define DEFAULT_PLAYER_NAME "NewPlayer"

#define SURFACE_Default		SurfaceType_Default
#define SURFACE_CementWet	SurfaceType1

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
	FGearHostInfo(const FString& InName, const FString& InIP, bool InHasPassword, int32 InNumPlayers, int32 InNumAllowedPlayers, int32 InWinningRequiredScore)
		: Name(InName)
		, IP(InIP)
		, bHasPassword(InHasPassword)
		, NumPlayers(InNumPlayers)
		, NumAllowedPlayers(InNumAllowedPlayers)
		, WinningRequiredScore(InWinningRequiredScore)
	{

	}

	FGearHostInfo() : FGearHostInfo("Host", "IP", false, 2, 4, 10)
	{

	}

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FString IP;

	UPROPERTY(BlueprintReadOnly)
	bool bHasPassword;

	UPROPERTY(BlueprintReadOnly)
	int32 NumPlayers;

	UPROPERTY(BlueprintReadOnly)
	int32 NumAllowedPlayers;

	UPROPERTY(BlueprintReadOnly)
	int32 WinningRequiredScore;
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
	TryingToStartGame,
	Timedout
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

USTRUCT(BlueprintType)
struct FCollisionBox
{
	GENERATED_BODY()

public:
	FCollisionBox() : FCollisionBox(FVector::ZeroVector, FRotator::ZeroRotator, FVector(128))
	{

	}

	FCollisionBox(const FVector& InLocation, const FRotator& InRotation, const FVector& InExtent)
		: Location(InLocation)
		, Rotation(InRotation)
		, Extent(InExtent)
	{

	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Location;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Extent;
};