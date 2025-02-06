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