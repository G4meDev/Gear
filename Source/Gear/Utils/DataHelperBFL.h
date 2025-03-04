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
	
	UFUNCTION(BlueprintPure)
	static FColor ResolveColorCode(EPlayerColorCode ColorCode);

	UFUNCTION(BlueprintPure)
	static UClass* GetHeadClassFromType(EDriverHead DriverHeadType);

	UFUNCTION(BlueprintPure)
	static void ConvertSecondsToMS(float Time, int32& Minutes, float& Seconds);

	UFUNCTION(BlueprintPure)
	static FString FormatTimeLong(float Time);
	
	UFUNCTION(BlueprintPure)
	static float RelativeToUnitPeriodWithCap(float RelativeTime, float Period, float Cap);

	UFUNCTION(BlueprintPure)
	static float SmoothStep(float Value, float Min, float Max);

	UFUNCTION(BlueprintPure)
	static FString LanguageOptionToCulture(ELanguageOption LanguageOption);

	UFUNCTION(BlueprintPure)
	static ELanguageOption CultureToLanguageOption(const FString& Culture);
	
	UFUNCTION(BlueprintPure)
	static FString ValidatePassword(const FString& Password);
};