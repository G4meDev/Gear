// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GearTypes.h"
#include "GearPlayerState.generated.h"

class AGearVehicle;

/**
 * 
 */
UCLASS()
class GEAR_API AGearPlayerState : public APlayerState
{
	GENERATED_BODY()


public:

	AGearPlayerState();

	UPROPERTY(ReplicatedUsing=OnRep_ColorCode, BlueprintReadWrite, EditAnywhere)
	EPlayerColorCode ColorCode;

	UFUNCTION()
	virtual void OnRep_ColorCode();

	UFUNCTION(BlueprintPure)
	int32 GetRoundBonusScore();

	UFUNCTION(BlueprintPure)
	void GetRoundScoreToCheckpoint(int32 CheckpointIndex, int32& ScoreToCheckpoint, int32& CheckpointScore);

	void UpdateRoundScore(const TArray<FCheckpointResult>& RoundScore);

	bool IsWinner() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FColor PlayerColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AGearVehicle> VehicleClass;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	int32 LastRoundScore;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	int32 CurrentScore;

	TArray<int> CheckpointsScore;
	int32 BonusScore;

protected:

	void OnRep_PlayerName() override;

	void CopyProperties(APlayerState* PlayerState) override;


};
