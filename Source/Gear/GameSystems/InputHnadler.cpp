// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/InputHnadler.h"

FMouseInputHandler::FMouseInputHandler()
{
	Init(EKeys::LeftMouseButton);
}

void FMouseInputHandler::Init(const FKey& InKey)
{
	Key = InKey;
	bHoldingKey = false;
	Velocity = FVector2D::ZeroVector;
	Value = FVector2D::ZeroVector;
}

void FMouseInputHandler::Tick(APlayerController* PC)
{
	if (IsValid(PC))
	{
		bool bKeyDown = PC->IsInputKeyDown(Key);

		bPressed = bKeyDown && !bHoldingKey;
		bReleased = !bKeyDown && bHoldingKey;

		FVector2D KeyPosition = FVector2D(0.0f);
		PC->GetMousePosition(KeyPosition.X, KeyPosition.Y);

		if (!bKeyDown)
		{
			Velocity = FVector2D::ZeroVector;
			Value = FVector2D::ZeroVector;
			bHoldingKey = false;
		}

		else if (bKeyDown && !bHoldingKey)
		{
			Velocity = FVector2D::ZeroVector;
			Value = KeyPosition;
			bHoldingKey = true;
		}

		else
		{
			Velocity = KeyPosition - Value;
			Value = KeyPosition;
		}
	}
}

const FVector2D& FMouseInputHandler::GetVelocity() const
{
	return Velocity;
}

const FVector2D& FMouseInputHandler::GetValue() const
{
	return Value;
}

bool FMouseInputHandler::IsHoldingKey() const
{
	return bHoldingKey;
}

bool FMouseInputHandler::Pressed()
{
	return bPressed;
}

bool FMouseInputHandler::Released()
{
	return bReleased;
}

// ----------------------------------------------------------------------

FTouchInputHandler::FTouchInputHandler()
{
	Init(ETouchIndex::Touch1);
}

void FTouchInputHandler::Init(ETouchIndex::Type InTouchIndex)
{
	TouchIndex = InTouchIndex;
	bHoldingTouch = false;
	Velocity = FVector2D::ZeroVector;
	Value = FVector2D::ZeroVector;
}

void FTouchInputHandler::Tick(APlayerController* PC)
{
	if (IsValid(PC))
	{
		bool bTouchDown;
		FVector2D KeyPosition = FVector2D(0.0f);
		PC->GetInputTouchState(TouchIndex, KeyPosition.X, KeyPosition.Y, bTouchDown);

		bPressed = bTouchDown && !bHoldingTouch;
		bReleased = !bTouchDown && bHoldingTouch;

		if (!bTouchDown)
		{
			Velocity = FVector2D::ZeroVector;
			Value = FVector2D::ZeroVector;
			bHoldingTouch = false;
		}

		else if (bTouchDown && !bHoldingTouch)
		{
			Velocity = FVector2D::ZeroVector;
			Value = KeyPosition;
			bHoldingTouch = true;
		}

		else
		{
			Velocity = KeyPosition - Value;
			Value = KeyPosition;
		}
	}
}

const FVector2D& FTouchInputHandler::GetVelocity() const
{
	return Velocity;
}

const FVector2D& FTouchInputHandler::GetValue() const
{
	return Value;
}

bool FTouchInputHandler::IsHoldingTouch() const
{
	return bHoldingTouch;
}

bool FTouchInputHandler::Pressed()
{
	return bPressed;
}

bool FTouchInputHandler::Released()
{
	return bReleased;
}