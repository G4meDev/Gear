// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameVariablesBFL.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UGameVariablesBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = GameVariables)
	static float GV_AllPlayerJoinToGameStartDelay();

	UFUNCTION(BlueprintPure, Category = GameVariables)
	static float GV_PieceSelectionTimeLimit();
};