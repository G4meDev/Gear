// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/GearTypes.h"
#include "DataHelperBFL.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UDataHelperBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	
public:
	
	UFUNCTION(BlueprintCallable)
	static FColor ResolveColorCode(EPlayerColorCode ColorCode);
};
