// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ScoreboardEntryWidget.h"
#include "UI/ScoreboardCheckpoint.h"
#include "UI/ScoreboardCheckpointSegment.h"
#include "UI/ScoreboardWidget.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearGameState.h"
#include "Utils/GameVariablesBFL.h"
#include "Components/PanelWidget.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"

void UScoreboardEntryWidget::InitWidgetForPlayer(UScoreboardWidget* InOwningScoreboard, AGearPlayerState* InPlayer)
{
	OwningScoreboard = InOwningScoreboard;
	Player = InPlayer;

	AGearGameState* GearGameState = IsValid(GetWorld()) ? GetWorld()->GetGameState<AGearGameState>() : nullptr;
	if (IsValid(GearGameState))
	{
		for (int i = 0; i < GearGameState->RoundsResult.Num() - 1; i++)
		{
			for (const FCheckpointResult& CheckpointResult : GearGameState->RoundsResult[i].CheckpointArray)
			{
				int PlayerPlacement = CheckpointResult.PlayerList.Find(Player);

				if (PlayerPlacement > INDEX_NONE)
				{
					if (PlayerPlacement == 0)
					{
						for (int k = 0; k < UGameVariablesBFL::GV_FirstFinishAdditionalScore(); k++)
						{
							UScoreboardCheckpointSegment* CheckpointSegment = GetCheckpointSegmentAtIndex(LastRoundsIndex);
							if (IsValid(CheckpointSegment))
							{
								CheckpointSegment->SetSegmentState(true, false, true);
								LastRoundsIndex++;
							}
						}
					}

					for (int k = 0; k < UGameVariablesBFL::GV_FinishingCheckpointScore(); k++)
					{
						UScoreboardCheckpointSegment* CheckpointSegment = GetCheckpointSegmentAtIndex(LastRoundsIndex);
						if (IsValid(CheckpointSegment))
						{
							CheckpointSegment->SetSegmentState(true, false, false);
							LastRoundsIndex++;
						}
					}
				}
			}
		}
	}
}

void UScoreboardEntryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Checkpoints.Empty(UGameVariablesBFL::GV_WinningRequiredScore() / 4);

	for (int i = 0; i < UGameVariablesBFL::GV_WinningRequiredScore() / 4; i++)
	{
		UScoreboardCheckpoint* Checkpoint = CreateWidget<UScoreboardCheckpoint>(this, ScoreboardCheckpointClass);

		Checkpoints.Add(Checkpoint);
		CheckpointHorizontalBox->AddChild(Checkpoint);
	}
}

void UScoreboardEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();


}

void UScoreboardEntryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	int32 Step = 0;
	
	auto ShouldBeVisible = [&]()
		{
			return Step <= OwningScoreboard->CurrentStep;
		};

	if (IsValid(Player) && IsValid(OwningScoreboard) && IsValid(OwningScoreboard->GearGameState))
	{
		for (const FCheckpointResult& Checkpoint : OwningScoreboard->GearGameState->RoundsResult.Last().CheckpointArray)
		{
			int32 PlayerIndex = Checkpoint.PlayerList.Find(Player);

			if (PlayerIndex > INDEX_NONE)
			{
				if (PlayerIndex == 0)
				{
					for (int k = 0; k < UGameVariablesBFL::GV_FirstFinishAdditionalScore(); k++)
					{
						UScoreboardCheckpointSegment* CheckpointSegment = GetCheckpointSegmentAtIndex(LastRoundsIndex + Step);
						if (IsValid(CheckpointSegment))
						{
							CheckpointSegment->SetSegmentState(ShouldBeVisible(), true, true);
							Step++;
						}
					}
				}

				for (int k = 0; k < UGameVariablesBFL::GV_FinishingCheckpointScore(); k++)
				{
					UScoreboardCheckpointSegment* CheckpointSegment = GetCheckpointSegmentAtIndex(LastRoundsIndex + Step);
					if (IsValid(CheckpointSegment))
					{
						CheckpointSegment->SetSegmentState(ShouldBeVisible(), true, false);
						Step++;
					}
				}
			}
		}
	}
}

ESlateVisibility UScoreboardEntryWidget::GetWidgetVisibility()
{
	return IsValid(Player) ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;
}

FLinearColor UScoreboardEntryWidget::GetPlayerColor()
{
	return IsValid(Player) ? Player->PlayerColor : FColor::White;
}

FText UScoreboardEntryWidget::GetPlayerName()
{
	return IsValid(Player) ? FText::FromString(Player->GetPlayerName()) : FText::FromString("");
}

UScoreboardCheckpointSegment* UScoreboardEntryWidget::GetCheckpointSegmentAtIndex(int32 Index)
{
	int32 i = Index / 4;
	int32 j = Index % 4;

	if (i < Checkpoints.Num() && IsValid(Checkpoints[i]))
	{
		return Checkpoints[i]->GetCheckpointSegmentAtIndex(j);
	}

	return nullptr;
}