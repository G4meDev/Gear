// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ScoreboardCheckpointSegment.h"
#include "UI/ScoreboardCheckpoint.h"
#include "Components/PanelWidget.h"
#include "Components/Image.h"

void UScoreboardCheckpointSegment::InitWidget(UScoreboardCheckpoint* InOwningScoreboardCheckpoint)
{
	OwningScoreboardCheckpoint = InOwningScoreboardCheckpoint;
}

void UScoreboardCheckpointSegment::SetSegmentState(bool bVisible, bool bTransparent, bool bGold)
{
	bool bShoulsPlayAnim = CheckpointSegmentImage->GetVisibility() == ESlateVisibility::Hidden && bVisible && bTransparent;

	if (bVisible && CheckpointSegmentImage->GetVisibility() == ESlateVisibility::Hidden)
	{
		CheckpointSegmentImage->SetVisibility(bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
		CheckpointSegmentImage->SetColorAndOpacity(bTransparent ? FLinearColor::White : FLinearColor(0.5f, 0.5f, 0.5f, 1.0f));
		CheckpointSegmentImage->SetBrush(bGold ? GoldCheckpointSegmentBrush : CheckpointSegmentBrush);
	}

	if (bShoulsPlayAnim)
	{
		PlayAnimation(ShakeAnim);
	}
}

void UScoreboardCheckpointSegment::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UScoreboardCheckpointSegment::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}