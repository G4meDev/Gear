// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/DataHelperBFL.h"

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