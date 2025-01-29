// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreboardEntryWidget.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UScoreboardEntryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UScoreboardWidget> OwningScoreboard;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> ColorBadgeImage;

public:

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class AGearPlayerState> Player;

	void InitWidget(UScoreboardWidget* InOwningScoreboard);

protected:
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintPure)
	ESlateVisibility GetWidgetVisibility();
};
