// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/DataHelperBFL.h"
#include "Vehicle/DriverHead.h"
#include "Kismet/KismetMathLibrary.h"

#define HEADPATH_BOX TEXT("/Game/Driver/DriverHead_Box/BP_DriverHead_Box.BP_DriverHead_Box_C")

FColor UDataHelperBFL::ResolveColorCode(EPlayerColorCode ColorCode)
{
	switch (ColorCode)
	{
		case EPlayerColorCode::Black:
			return FColor(0, 0, 0);

		case EPlayerColorCode::NileBlue:
			return FColor(28, 43, 83);

		case EPlayerColorCode::PansyPurple:
			return FColor(127, 36, 84);

		case EPlayerColorCode::ElfGreen:
			return FColor(0, 135, 81);

		case EPlayerColorCode::BrownRust:
			return FColor(171, 82, 54);

		case EPlayerColorCode::Wenge:
			return FColor(96, 87, 59);

		case EPlayerColorCode::Cloud:
			return FColor(195, 195, 198);

		case EPlayerColorCode::Linen:
			return FColor(255, 241, 233);

		case EPlayerColorCode::RedRibbon:
			return FColor(237, 27, 81);

		case EPlayerColorCode::Squash:
			return FColor(250, 162, 27);

		case EPlayerColorCode::BananaYellow:
			return FColor(247, 236, 47);

		case EPlayerColorCode::GreenSnake:
			return FColor(93, 187, 77);

		case EPlayerColorCode::PictonBlue:
			return FColor(81, 166, 220);

		case EPlayerColorCode::GrayishPurple:
			return FColor(131, 118, 156);

		case EPlayerColorCode::DeepBlush:
			return FColor(241, 118, 166);

		case EPlayerColorCode::DeepPeach:
			return FColor(252, 204, 171);

		default:
			check(false);
			return FColor::Black;
	}
}

UClass* UDataHelperBFL::GetHeadClassFromType(EDriverHead DriverHeadType)
{
	switch (DriverHeadType)
	{
	case EDriverHead::Box:
		return StaticLoadClass(ADriverHead::StaticClass(), nullptr, HEADPATH_BOX);
	default:
		return nullptr;
	}
}

void UDataHelperBFL::ConvertSecondsToMS(float Time, int32& Minutes, float& Seconds)
{
	Minutes = Time / 60.0f;
	Seconds = Time - (Minutes * 60.0f);
}

FString UDataHelperBFL::FormatTimeLong(float Time)
{
	int32 Minutes;
	float Seconds;
	ConvertSecondsToMS(Time, Minutes, Seconds);
	
	return FString::Printf(TEXT("%.2i:%.3f"), Minutes, Seconds);
}

float UDataHelperBFL::RelativeToUnitPeriodWithCap(float RelativeTime, float Period, float Cap)
{
	float Result = FMath::Fmod(RelativeTime, Period);
	float CapRatio = Cap / Period; 

	Result = FMath::Abs((Result / Period) - 0.5f) * 2;
	Result = UKismetMathLibrary::MapRangeClamped(Result, CapRatio, 1 - CapRatio, 0, 1);

	return Result;
}

float UDataHelperBFL::SmoothStep(float Value, float Min, float Max)
{
	return FMath::SmoothStep(Min, Max, Value);
}

FString UDataHelperBFL::LanguageOptionToCulture(ELanguageOption LanguageOption)
{
	switch (LanguageOption)
	{
	case ELanguageOption::Farsi:
		return FString("fa");
	case ELanguageOption::English:
		return FString("en");
	default:
		return FString("fa");
	}
}

ELanguageOption UDataHelperBFL::CultureToLanguageOption(const FString& Culture)
{
	if (Culture == "fa")
	{
		return ELanguageOption::Farsi;
	}

	if (Culture == "en")
	{
		return ELanguageOption::English;
	}

	return ELanguageOption::Farsi;
}

FString UDataHelperBFL::ValidatePassword(const FString& Password)
{
	FString Result = Password;
	Result = Result.Replace(TEXT("\n"), TEXT(""));
	Result = Result.Replace(TEXT("\t"), TEXT(""));
	Result = Result.Replace(TEXT(" "), TEXT(""));

	Result = Result.Left(8);

	return Result;
}