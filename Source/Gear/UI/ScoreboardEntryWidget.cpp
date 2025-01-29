// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ScoreboardEntryWidget.h"
#include "GameFramework/GearPlayerState.h"
#include "Components/PanelWidget.h"
#include "Components/Image.h"

void UScoreboardEntryWidget::InitWidget(UScoreboardWidget* InOwningScoreboard)
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

ESlateVisibility UScoreboardEntryWidget::GetWidgetVisibility()
{
	return IsValid(Player) ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;
}