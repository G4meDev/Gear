// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearHUD.h"
#include "GameFramework/GearPlayerState.h"
#include "Components/PanelWidget.h"
#include "UI/GearBaseWidget.h"
#include "UI/VehicleInputWidget.h"
#include "Utils/GameVariablesBFL.h"
#include "Blueprint/UserWidget.h"

AGearHUD::AGearHUD()
{
	bPaused = false;
}

void AGearHUD::BeginPlay()
{
	Super::BeginPlay();

	AddWidget(ScreenMenuWidgetClass, ScreenMenuWidget, 0);
	AddWidget(NotifictionWidgetClass, NotifictionWidget, 0);
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

void AGearHUD::AddVehicleInputWidget(class AGearVehicle* OwningVehicle)
{
	AddWidget(VehicleInputWidgetClass, VehicleInputWidget, 0);

	UVehicleInputWidget* InputWidget = Cast<UVehicleInputWidget>(VehicleInputWidget);
	if (IsValid(InputWidget))
	{
		InputWidget->OwningVehicle = OwningVehicle;
	}
}

void AGearHUD::RemoveVehicleInputWidget()
{
	RemoveWidget(VehicleInputWidget);
}

void AGearHUD::AddWidget(TSubclassOf<UGearBaseWidget> WidgetClass, UGearBaseWidget*& Widget, float InStartTime)
{
	if (!IsValid(Widget))
	{
		if (IsValid(WidgetClass))
		{
			Widget = CreateWidget<UGearBaseWidget>(GetWorld(), WidgetClass);
		}

		if (IsValid(Widget))
		{
			if (IsPaused())
			{
				Widget->SetVisibility(ESlateVisibility::Hidden);
			}

			Widget->OwningHUD = this;
			Widget->StartTime = InStartTime;
			WidgetStack.Add(Widget);
			
			Widget->AddToViewport();
			//ReconstructWidgetsOrder();
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
	WidgetStack.StableSort(
		[](const UGearBaseWidget& A, const UGearBaseWidget& B)
		{
			return static_cast<uint8>(A.WidgetOrder) < static_cast<uint8>(B.WidgetOrder);
		});

	for (int i = 0; i < WidgetStack.Num(); i++)
	{
		WidgetStack[i]->RemoveFromParent();
		WidgetStack[i]->AddToViewport();
	}
}

void AGearHUD::SetVisilityOfWidgetsInStack(bool bVisible)
{
	for (UGearBaseWidget* Widget : WidgetStack)
	{
		Widget->SetVisibility(bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}
}

// -------------------------------------------------------------------------------------------------------

void AGearHUD::Waiting_Start(float StartTime)
{
	AddWidget(WaitingWidgetClass, WaitingWidget, StartTime);
}

void AGearHUD::Waiting_End()
{
	RemoveWidget(WaitingWidget);
}

void AGearHUD::Selecting_Start(float StartTime)
{
	AddWidget(SelectingWidgetClass, SelectingWidget, StartTime);
}

void AGearHUD::Selecting_End()
{
	RemoveWidget(SelectingWidget);
}

void AGearHUD::Placing_Start(float StartTime)
{
	AddWidget(PlacingWidgetClass, PlacingWidget, StartTime);
}

void AGearHUD::Placing_End()
{
	RemoveWidget(PlacingWidget);
}

void AGearHUD::Racing_Start(float StartTime)
{
	AddWidget(RacingWidgetClass, RacingWidget, StartTime);
}

void AGearHUD::Racing_End()
{
	RemoveWidget(RacingWidget);
}

void AGearHUD::Scoreboard_Start(float StartTime)
{
	AddWidget(ScoreboardWidgetClass, ScoreboardWidget, StartTime);
}

void AGearHUD::Scoreboard_End()
{
	RemoveWidget(ScoreboardWidget);
}

void AGearHUD::Finishboard_Start(float StartTime)
{
	AddWidget(FinishboardWidgetClass, FinishboardWidget, StartTime);
}

void AGearHUD::Finishboard_End()
{
	RemoveWidget(FinishboardWidget);
}

void AGearHUD::Pause_Start(float StartTime)
{
	SetVisilityOfWidgetsInStack(false);
	AddWidget(PauseWidgetClass, PauseWidget, StartTime);
	bPaused = true;
}

void AGearHUD::Pause_End()
{
	RemoveWidget(PauseWidget);
	SetVisilityOfWidgetsInStack(true);
	bPaused = false;
}

bool AGearHUD::IsPaused()
{
	return bPaused;
}

void AGearHUD::AllPlayersJoined()
{
	OnAllPlayersJoined.Broadcast();
}

void AGearHUD::PlayerJoined(class AGearPlayerState* Player)
{
	OnPlayerJoined.Broadcast(Player);
}

void AGearHUD::PlayerQuit(class AGearPlayerState* Player)
{
	OnPlayerQuit.Broadcast(Player);
}

void AGearHUD::PlayerEliminated(AGearPlayerState* Player, EElimanationReason ElimanationReason, float EliminationTime, int32 RemainingPlayersCount)
{
	OnPlayerEliminated.Broadcast(Player, ElimanationReason, EliminationTime, RemainingPlayersCount);
}

void AGearHUD::ReachedCheckpoint(AGearPlayerState* Player, class ACheckpoint* Checkpoint, int32 Position, int32 AllCheckpointNum, float ReachTime)
{
	OnReachedCheckpoint.Broadcast(Player, Checkpoint, Position, AllCheckpointNum, ReachTime);
}

void AGearHUD::RaceStart(float StartTime, bool bWithCountDown)
{
	OnRaceStart.Broadcast(StartTime, bWithCountDown);
}

void AGearHUD::AbilityStateChanged(class AGearAbility* Ability)
{
	OnAbilityStateChanged.Broadcast(Ability);
}
