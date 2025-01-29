// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ScoreboardCheckpoint.h"
#include "UI/ScoreboardCheckpointSegment.h"

void UScoreboardCheckpoint::InitWidget(UScoreboardEntryWidget* InOwningScoreboardEntry)
{
	OwningScoreboardEntry = InOwningScoreboardEntry;
}

void UScoreboardCheckpoint::NativeConstruct()
{
	Super::NativeConstruct();

	CheckpointSegment_1->InitWidget(this);
	CheckpointSegment_2->InitWidget(this);
	CheckpointSegment_3->InitWidget(this);
	CheckpointSegment_4->InitWidget(this);
}