// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ScoreboardCheckpoint.h"
#include "UI/ScoreboardCheckpointSegment.h"

void UScoreboardCheckpoint::InitWidget(UScoreboardEntryWidget* InOwningScoreboardEntry)
{
	OwningScoreboardEntry = InOwningScoreboardEntry;
}

UScoreboardCheckpointSegment* UScoreboardCheckpoint::GetCheckpointSegmentAtIndex(int32 Index)
{
	switch (Index)
	{
	case 0:
		return CheckpointSegment_1;

	case 1:
		return CheckpointSegment_2;

	case 2:
		return CheckpointSegment_3;

	case 3:
		return CheckpointSegment_4;

	default:
		return nullptr;
	}
}

void UScoreboardCheckpoint::NativeConstruct()
{
	Super::NativeConstruct();

	CheckpointSegment_1->InitWidget(this);
	CheckpointSegment_2->InitWidget(this);
	CheckpointSegment_3->InitWidget(this);
	CheckpointSegment_4->InitWidget(this);
}