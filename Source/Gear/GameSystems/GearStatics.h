// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/GearTypes.h"
#include "GearStatics.generated.h"

UCLASS()
class GEAR_API UGearStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static ERoadModuleTraceResult TraceRoadModule(UObject* WorldContextObject, TSubclassOf<class AGearRoadModule> RoadModuleClass, const FTransform& SocketTransform);
};
