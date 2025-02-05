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
	
	ALobbyPlayerState();
	void BeginPlay();
	void Destroyed() override;

	void OnRep_PlayerName() override;
	void CopyProperties(APlayerState* PlayerState) override;

	UPROPERTY(Replicated)
	float PlayerJoinTime;

	friend class ALobbyGameState;

public:

	UPROPERTY(ReplicatedUsing=OnRep_ColorCode, BlueprintReadWrite, EditAnywhere)
	EPlayerColorCode ColorCode;

	UFUNCTION()
	virtual void OnRep_ColorCode();


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FColor PlayerColor;


	UFUNCTION(BlueprintPure)
	float GetPlayerJoinTime();

protected:
};
