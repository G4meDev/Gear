// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreboardCheckpointSegment.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UScoreboardCheckpointSegment : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> CheckpointSegmentImage;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UScoreboardCheckpoint> OwningScoreboardCheckpoint; 

public:
	
	void InitWidget(UScoreboardCheckpoint* InOwningScoreboardCheckpoint);

protected:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

};