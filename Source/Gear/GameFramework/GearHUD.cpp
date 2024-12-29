// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearHUD.h"
#include "GameFramework/GearPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Utils/GameVariablesBFL.h"

void AGearHUD::ShowTransitionWidget()
{
	check(TransitaionWidgetClass);

	if (IsValid(TransitaionWidget))
	{
		TransitaionWidget->RemoveFromParent();
		GetWorld()->GetTimerManager().ClearTimer(TransitionTimerHandle);
	}

	TransitaionWidget = CreateWidget(GetWorld(), TransitaionWidgetClass);
	TransitaionWidget->AddToViewport(INT_MAX);
	
	GetWorld()->GetTimerManager().SetTimer(TransitionTimerHandle
		, FTimerDelegate::CreateLambda([&]()
		{
			TransitaionWidget->RemoveFromParent();
			TransitaionWidget = nullptr;
		})
		, UGameVariablesBFL::GV_TransitionDuration(), false);
}
