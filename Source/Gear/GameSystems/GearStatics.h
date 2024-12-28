// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GearStatics.generated.h"

UENUM()
enum class ERoadModuleTraceResult : uint8
{
	NotColliding,
	BodyColliding,
	ExtentColliding
};

UCLASS()
class GEAR_API UGearStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static ERoadModuleTraceResult TraceRoadModule(UObject* WorldContextObject, TSubclassOf<class AGearRoadModule> RoadModuleClass, const FTransform& SocketTransform);
};
