// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncTaskPing.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPingDelegate, float, Ping);

UCLASS()
class GEAR_API UAsyncTaskPing : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, meta=( BlueprintInternalUseOnly="true" ))
	static UAsyncTaskPing* PingUrl(FString URL, float Timeout);

	void Start(FString URL, float Timeout);

	double RequestStartTime;

	UPROPERTY(BlueprintAssignable)
	FPingDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FPingDelegate OnTimeout; 
};