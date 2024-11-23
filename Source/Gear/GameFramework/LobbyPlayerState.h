// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerNameChanged);

/**
 * 
 */
UCLASS()
class GEAR_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	void BeginPlay();

	void Destroyed() override;

	void OnRep_PlayerName() override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnPlayerNameChanged OnPlayerNameChanged;
};
