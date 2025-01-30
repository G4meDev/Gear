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
	CheckpointSegmentImage->SetVisibility(bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);

	if (bVisible)
	{
		CheckpointSegmentImage->SetColorAndOpacity(bTransparent ? FLinearColor(1.0f, 1.0f, 1.0f, 0.5f) : FLinearColor::White);
		CheckpointSegmentImage->SetBrush(bGold ? GoldCheckpointSegmentBrush : CheckpointSegmentBrush);
	}
}

void UScoreboardCheckpointSegment::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UScoreboardCheckpointSegment::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::Tick(MyGeometry, InDeltaTime);

}