// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearPlayerState.h"
#include "Placeable/GearPlaceable.h"
#include "Utils/DataHelperBFL.h"
#include "Utils/GameVariablesBFL.h"
#include "Net/UnrealNetwork.h"

AGearPlayerState::AGearPlayerState()
{
	ColorCode = EPlayerColorCode::Black;
	OnRep_ColorCode();

	LastRoundScore = 0;
	CurrentScore = 0;

	BonusScore = 0;
}

void AGearPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearPlayerState, ColorCode);
	DOREPLIFETIME(AGearPlayerState, LastRoundScore);
	DOREPLIFETIME(AGearPlayerState, CurrentScore);
}

void AGearPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();


}

void AGearPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	
}


void AGearPlayerState::OnRep_ColorCode()
{
	PlayerColor = UDataHelperBFL::ResolveColorCode(ColorCode);

}

int32 AGearPlayerState::GetRoundBonusScore()
{
	return BonusScore;
}

void AGearPlayerState::GetRoundScoreToCheckpoint(int32 CheckpointIndex, int32& ScoreToCheckpoint, int32& CheckpointScore)
{
	check(CheckpointIndex >= 0 && CheckpointIndex < CheckpointsScore.Num());

	ScoreToCheckpoint = 0;

	for (int32 i = 0; i < CheckpointIndex; i++)
	{
		ScoreToCheckpoint += CheckpointsScore[i];
	}

	CheckpointScore = CheckpointsScore[CheckpointIndex];
}

void AGearPlayerState::UpdateRoundScore(const TArray<FCheckpointResult>& RoundScore)
{
	int Temp = 0;

	CheckpointsScore.Empty(RoundScore.Num());
	BonusScore = 0;

	for (const FCheckpointResult& CheckpointResult : RoundScore)
	{
		if (CheckpointResult.PlayerList.Contains(this))
		{
			CheckpointsScore.Add(UGameVariablesBFL::GV_FinishingCheckpointScore());
			Temp += UGameVariablesBFL::GV_FinishingCheckpointScore();

			BonusScore += CheckpointResult.PlayerList.Find(this) == 0 ? UGameVariablesBFL::GV_FirstFinishAdditionalScore() : 0;
		}
		else
		{
			CheckpointsScore.Add(0);
		}
	}

	LastRoundScore = CurrentScore;
	CurrentScore += Temp + BonusScore;
}