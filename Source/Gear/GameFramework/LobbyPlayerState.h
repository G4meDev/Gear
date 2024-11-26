// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GearTypes.h"
#include "LobbyPlayerState.generated.h"

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

	UFUNCTION()
	virtual void OnRep_ColorCode(EPlayerColorCode OldColor);

	void CopyProperties(APlayerState* PlayerState) override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnPlayerNameChanged OnPlayerNameChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerColorChanged OnPlayerColorChanged;

	UPROPERTY(ReplicatedUsing=OnRep_ColorCode, BlueprintReadWrite, EditAnywhere)
	EPlayerColorCode ColorCode;

	FColor PlayerColor;

};
