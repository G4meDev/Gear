// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameframeWork/GearTypes.h"
#include "ScoreboardEntryWidget.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UScoreboardEntryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<class UScoreboardCheckpoint> ScoreboardCheckpointClass;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UScoreboardWidget> OwningScoreboard;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> ColorBadgeImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> CheckpointHorizontalBox;

	UPROPERTY()
	TArray<UScoreboardCheckpoint*> Checkpoints;

	int32 LastRoundsIndex = 0;

public:

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class AGearPlayerState> Player;

	void InitWidgetForPlayer(UScoreboardWidget* InOwningScoreboard, AGearPlayerState* InPlayer);

protected:
	
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintPure)
	ESlateVisibility GetWidgetVisibility();

	UFUNCTION(BlueprintPure)
	FLinearColor GetPlayerColor();

	UFUNCTION(BlueprintPure)
	FText GetPlayerName();

	class UScoreboardCheckpointSegment* GetCheckpointSegmentAtIndex(int32 Index);
};
