// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearHUD.h"
#include "GameFramework/GearPlayerState.h"
#include "Components/PanelWidget.h"
#include "UI/GearBaseWidget.h"
#include "Utils/GameVariablesBFL.h"
#include "Blueprint/UserWidget.h"

AGearHUD::AGearHUD()
{
	bPaused = false;
}

void AGearHUD::BeginPlay()
{
	Super::BeginPlay();

	AddWidget(ScreenMenuWidgetClass, ScreenMenuWidget);
}

void AGearHUD::Destroyed()
{
	Super::Destroyed();

	for (UGearBaseWidget* Widget : WidgetStack)
	{
		if (Widget->GetParent())
		{
			Widget->RemoveFromParent();
		}

		Widget = nullptr;
	}
}

void AGearHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AGearHUD::SelectingStart()
{
	AddWidget(SelectingWidgetClass, SelectingWidget);
}

void AGearHUD::Pause_Start()
{
	AddWidget(PauseWidgetClass, PauseWidget);
	bPaused = true;
}

void AGearHUD::Pause_End()
{
	RemoveWidget(PauseWidget);
	bPaused = false;
}

bool AGearHUD::IsPaused()
{
	return bPaused;
}

void AGearHUD::AddWidget(TSubclassOf<UGearBaseWidget> WidgetClass, UGearBaseWidget*& Widget)
{
	if (!IsValid(Widget))
	{
		if (IsValid(WidgetClass))
		{
			Widget = CreateWidget<UGearBaseWidget>(GetWorld(), WidgetClass);
		}

		if (IsValid(Widget))
		{
			Widget->OwningHUD = this;
			WidgetStack.Add(Widget);
			ReconstructWidgetsOrder();
		}
	}
}

void AGearHUD::RemoveWidget(UGearBaseWidget*& Widget)
{
	if (IsValid(Widget))
	{
		Widget->RemoveFromParent();
		WidgetStack.Remove(Widget);
		Widget = nullptr;
	}
}

void AGearHUD::ReconstructWidgetsOrder()
{
	WidgetStack.Sort(
		[](const UGearBaseWidget& A, const UGearBaseWidget& B)
		{
			return static_cast<uint8>(A.WidgetOrder) < static_cast<uint8>(B.WidgetOrder);
		});

	for (UGearBaseWidget* Widget : WidgetStack)
	{
		Widget->RemoveFromParent();
		Widget->AddToViewport();
	}
}