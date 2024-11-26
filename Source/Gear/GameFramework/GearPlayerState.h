// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GearTypes.h"
#include "GearPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API AGearPlayerState : public APlayerState
{
	GENERATED_BODY()


protected:

	void OnRep_PlayerName() override;

	void CopyProperties(APlayerState* PlayerState) override;

	UFUNCTION()
	virtual void OnRep_ColorCode();

public:

	UPROPERTY(BlueprintAssignable)
	FOnPlayerNameChanged OnPlayerNameChanged;

// 	UPROPERTY(ReplicatedUsing=OnRep_ColorCode)
// 	EPlayerColorCode ColorCode;

	FColor PlayerColor;
};
