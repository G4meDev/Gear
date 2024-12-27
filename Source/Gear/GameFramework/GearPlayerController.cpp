// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearGameState.h"
#include "GameFramework/GearGameMode.h"
#include "GameFramework/GearBuilderPawn.h"
#include "Vehicle/VehicleCamera.h"
#include "GearHUD.h"
#include "Placeable/GearPlaceable.h"
#include "GameSystems/TrackSpline.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

AGearPlayerController::AGearPlayerController()
{
	bAutoManageActiveCameraTarget = false;

	IsReady = false;

	bDraging = false;
	LastDragPosition = FVector2D::Zero();
}

void AGearPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AGearPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IsValid(InputMappingContext))
			{
				InputSystem->AddMappingContext(InputMappingContext, 0);
			}
		}


		AGearHUD* GearHUD = GetHUD<AGearHUD>();
		if (IsValid(GearHUD))
		{
			GearHUD->ShowWaitingScreen();
		}

		for(APlayerState* State : GetWorld()->GetGameState()->PlayerArray)
		{
			AGearPlayerState* GearPlayer = Cast<AGearPlayerState>(State);
			if (IsValid(GearPlayer))
			{
				OnNewPlayer(GearPlayer);
			}
		}
	}


}

void AGearPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocalController())
	{
		UpdateScreenDragValueAndInjectInput();

		
	}
}

void AGearPlayerController::UpdateScreenDragValueAndInjectInput()
{
	bool bMouseDown = IsInputKeyDown(EKeys::LeftMouseButton);
	FVector2D MousePosition;
	GetMousePosition(MousePosition.X, MousePosition.Y);
	
	bool bTouchDown;
	FVector2D TouchPosition;
	GetInputTouchState(ETouchIndex::Touch1, TouchPosition.X, TouchPosition.Y, bTouchDown);

	bool HoldingScreen = bMouseDown || bTouchDown;

	FVector2D CurrentPosition = FVector2D::Zero();

	if (bMouseDown)
	{
		CurrentPosition = MousePosition;
	}
	
	else if (bTouchDown)
	{
		CurrentPosition = TouchPosition;
	}

	FVector2D ScreenDragValue;

	if (!HoldingScreen)
	{
		ScreenDragValue = FVector2D::Zero();
		LastDragPosition = FVector2D::Zero();
		bDraging = false;
	}

	// just started draging
	else if (HoldingScreen && !bDraging)
	{
		ScreenDragValue = FVector2D::Zero();
		LastDragPosition = CurrentPosition;
		bDraging = true;
	}

	else
	{
		ScreenDragValue = CurrentPosition - LastDragPosition;
		LastDragPosition = CurrentPosition;
		bDraging = true;
	}

	if (UEnhancedInputLocalPlayerSubsystem* InputSystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		if (IsValid(MoveScreenAction))
		{
// 			int32 SizeX;
// 			int32 SizeY;
// 			GetViewportSize(SizeX, SizeY);
// 
// 			ScreenDragValue /= FVector2D(SizeX, SizeY);
			
			TArray<UInputModifier*> Modifiers;
			TArray<UInputTrigger*> Triggers;

			InputSystem->InjectInputForAction(MoveScreenAction, FInputActionValue(FInputActionValue::Axis2D(ScreenDragValue)), Modifiers, Triggers);
		}
	}

}

void AGearPlayerController::ClientStateAllPlayersJoined()
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->AllPlayersJoined();
	}
}

void AGearPlayerController::ClientStateMatchStarted()
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->RemoveWaitingScreen();
	}
}

void AGearPlayerController::SelectPlaceable_Implementation(AGearPlaceable* Placeable)
{
	AGearBuilderPawn* BuilderPawn = GetPawn<AGearBuilderPawn>();

	if (IsValid(BuilderPawn))
	{
		AGearGameMode* GearGameMode = Cast<AGearGameMode>(GetWorld()->GetAuthGameMode());

		if (IsValid(GearGameMode))
		{
			GearGameMode->RequestSelectingPlaceableForPlayer(Placeable, BuilderPawn);
		}
	}
}

void AGearPlayerController::PlaceRoadModule_Implementation(TSubclassOf<AGearRoadModule> RoadModule, bool bMirrorX)
{
	AGearGameMode* GearGameMode = Cast<AGearGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(GearGameMode))
	{
		GearGameMode->RequestPlaceRoadModuleForPlayer(this, RoadModule, bMirrorX);
	}
}

void AGearPlayerController::ClientStateSelectingPieces(float StateStartTime)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->StartSelectingPieces(StateStartTime);
	}
}

void AGearPlayerController::ClientStatePlacing(float StateStartTime)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->StartPlacingPieces(StateStartTime);
	}
}



void AGearPlayerController::ClientStatePlacing_Finish()
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->PlacingPieces_End();
	}
}

void AGearPlayerController::ClientStateRacing_Start(float StateStartTime)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->Racing_Start(StateStartTime);
	}


}

void AGearPlayerController::ClientStateRacing_End()
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->Racing_End();
	}
}

void AGearPlayerController::ClientStateScoreboard_Start(float StateStartTime, const TArray<FCheckpointResult>& RoundResults)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->Scoreboard_Start(StateStartTime, RoundResults);
	}
}

void AGearPlayerController::ClientStateScoreboard_End()
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->Scoreboard_End();
	}
}

void AGearPlayerController::ClientStateGameFinished(float StateStartTime)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->GameFinished(StateStartTime);
	}
}

void AGearPlayerController::PeekClientIsReady_Implementation()
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		RespondClientIsReady();
	}
}

void AGearPlayerController::RespondClientIsReady_Implementation()
{
	IsReady = true;
}

void AGearPlayerController::OnNewPlayer(AGearPlayerState* GearPlayer)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->AddPlayer(GearPlayer);
	}
}

void AGearPlayerController::OnRemovePlayer(AGearPlayerState* GearPlayer)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->RemovePlayer(GearPlayer);
	}
}