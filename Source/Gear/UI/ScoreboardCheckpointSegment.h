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

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FSlateBrush CheckpointSegmentBrush;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FSlateBrush GoldCheckpointSegmentBrush;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> CheckpointSegmentImage;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> ShakeAnim;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UScoreboardCheckpoint> OwningScoreboardCheckpoint; 

public:
	
	void InitWidget(UScoreboardCheckpoint* InOwningScoreboardCheckpoint);

	void SetSegmentState(bool bVisible, bool bTransparent, bool bGold);

protected:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

};