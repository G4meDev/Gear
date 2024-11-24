// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GearTypes.generated.h"

#define DEFAULT_PLAYER_NAME "NewPlayer"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerNameChanged);

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
