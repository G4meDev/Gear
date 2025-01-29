// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ScoreboardEntryWidget.h"

void UScoreboardEntryWidget::InitWidget(UGearBaseWidget* InOwningScoreboard)
{
	OwningScoreboard = InOwningScoreboard;
}

void UScoreboardEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();


}

void UScoreboardEntryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}