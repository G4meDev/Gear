// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FMouseInputHandler
{	
protected:

	FKey Key;
	FVector2D Velocity;
	FVector2D Value;
	bool bHoldingKey;

	bool bPressed;
	bool bReleased;

public:
	FMouseInputHandler();

	void Init(const FKey& InKey);

	void Tick(APlayerController* PC);

	const FVector2D& GetVelocity() const;
	const FVector2D& GetValue() const;

	bool IsHoldingKey() const;

	bool Pressed();
	bool Released();
};


struct FTouchInputHandler
{
protected:

	ETouchIndex::Type TouchIndex;
	FVector2D Velocity;
	FVector2D Value;
	bool bHoldingTouch;

	bool bPressed;
	bool bReleased;

public:
	FTouchInputHandler();

	void Init(ETouchIndex::Type InTouchIndex);

	void Tick(APlayerController* PC);

	const FVector2D& GetVelocity() const;
	const FVector2D& GetValue() const;

	bool IsHoldingTouch() const;

	bool Pressed();
	bool Released();
};