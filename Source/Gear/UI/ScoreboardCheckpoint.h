// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreboardCheckpoint.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UScoreboardCheckpoint : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UScoreboardEntryWidget> OwningScoreboardEntry;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UScoreboardCheckpointSegment> CheckpointSegment_1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UScoreboardCheckpointSegment> CheckpointSegment_2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UScoreboardCheckpointSegment> CheckpointSegment_3;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UScoreboardCheckpointSegment> CheckpointSegment_4;

public:
	void InitWidget(UScoreboardEntryWidget* InOwningScoreboardEntry);

protected:
	virtual void NativeConstruct() override;

};