// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/GameVariablesBFL.h"

float UGameVariablesBFL::GV_AllPlayerJoinToGameStartDelay()
{
	return 4.0f;
}

float UGameVariablesBFL::GV_CountDownDuration()
{
	return 2.0f;
}

float UGameVariablesBFL::GV_InvincibilityDuration()
{
	return 3.0f;
}

float UGameVariablesBFL::GV_PieceSelectionTimeLimit()
{
	return 10.0f;
}

float UGameVariablesBFL::GV_PlacingTimeLimit()
{
	return 10.0f;
}

int32 UGameVariablesBFL::GV_FinishingCheckpointScore()
{
	return 150;
}

int32 UGameVariablesBFL::GV_FirstFinishAdditionalScore()
{
	return 100;
}

int32 UGameVariablesBFL::GV_WinningRequiredScore()
{
	return 3000;
}

float UGameVariablesBFL::GV_ScoreboardTimeStep()
{
	return 2.0f;
}

float UGameVariablesBFL::GV_ScoreboardTimeDelay()
{
	return 0.7f;
}

float UGameVariablesBFL::GV_TransitionDuration()
{
	return 1.0f;
}