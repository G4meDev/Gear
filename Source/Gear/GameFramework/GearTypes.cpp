// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GearTypes.h"
#include "Utils/DataHelperBFL.h"

void FCustomizationColor::ResolveColorCode()
{
	Color = UDataHelperBFL::ResolveColorCode(ColorCode);
	LinearColor = Color.ReinterpretAsLinear();
}

void FCustomizationHead::ResolveClass()
{
	Class = UDataHelperBFL::GetHeadClassFromType(Type);
}

FPlayerCustomization FPlayerCustomization::GetRandomCustomization()
{
	auto GetRandomColorCode = [&]()
		{
			uint8 Index = FMath::RandRange(0, static_cast<uint8>(EPlayerColorCode::MAX_COLOR) - 1);
			return static_cast<EPlayerColorCode>(Index);
		};

	auto GetRandomHeadType = [&]()
		{
			uint8 Index = FMath::RandRange(0, static_cast<uint8>(EDriverHead::Max) - 1);
			return static_cast<EDriverHead>(Index);
		};

	FPlayerCustomization RandomCustomization;

	RandomCustomization.TricycleColor.ColorCode = GetRandomColorCode();
	RandomCustomization.ClothColor.ColorCode = GetRandomColorCode();
	RandomCustomization.PantColor.ColorCode = GetRandomColorCode();
	RandomCustomization.HandColor.ColorCode = GetRandomColorCode();
	RandomCustomization.ShoeColor.ColorCode = GetRandomColorCode();

	RandomCustomization.HeadType.Type = GetRandomHeadType();

	return RandomCustomization;
}
