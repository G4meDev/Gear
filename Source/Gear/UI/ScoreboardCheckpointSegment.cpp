// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ScoreboardCheckpointSegment.h"
#include "UI/ScoreboardCheckpoint.h"
#include "Components/PanelWidget.h"

void UScoreboardCheckpointSegment::InitWidget(UScoreboardCheckpoint* InOwningScoreboardCheckpoint)
{
	OwningScoreboardCheckpoint = InOwningScoreboardCheckpoint;
}

void UScoreboardCheckpointSegment::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UScoreboardCheckpointSegment::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::Tick(MyGeometry, InDeltaTime);

}