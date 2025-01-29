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
	TObjectPtr<class AGearPlayerState> Player;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<class UGearBaseWidget> OwningScoreboard;

public:

	UFUNCTION(BlueprintCallable)
	void InitWidget(UGearBaseWidget* InOwningScoreboard);

protected:
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
