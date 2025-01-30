// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GearBaseWidget.h"
#include "ScoreboardWidget.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UScoreboardWidget : public UGearBaseWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UScoreboardEntryWidget> Entry_1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UScoreboardEntryWidget> Entry_2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UScoreboardEntryWidget> Entry_3;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UScoreboardEntryWidget> Entry_4;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	USoundBase* StepSound;

	float EstimatedLifeSpan = FLT_MAX;

public:

	int32 CurrentStep = -1;
	int32 LastFrameStep = -1;

	UPROPERTY()
	class AGearGameState* GearGameState;

protected:
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void AddPlayer(AGearPlayerState* Player);

	UFUNCTION()
	void RemovePlayer(AGearPlayerState* Player);
};