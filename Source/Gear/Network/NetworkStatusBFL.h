// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NetworkStatusBFL.generated.h"

USTRUCT(BlueprintType)
struct FNetworkStatus
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	int32 Ping;

	UPROPERTY(BlueprintReadWrite)
	int32 OutPacketsPerSecond;

	UPROPERTY(BlueprintReadWrite)
	int32 OutBytesPerSecond;

	UPROPERTY(BlueprintReadWrite)
	int32 OutPacketsLost;

	UPROPERTY(BlueprintReadWrite)
	int32 InPacketsPerSecond;

	UPROPERTY(BlueprintReadWrite)
	int32 InBytesPerSecond;

	UPROPERTY(BlueprintReadWrite)
	int32 InPacketsLost;

};

/**
 * 
 */
UCLASS()
class GEAR_API UNetworkStatusBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintPure)
	static FNetworkStatus GetNetworkStatus(APlayerController* PC);
};
