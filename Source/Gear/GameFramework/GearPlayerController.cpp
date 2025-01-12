// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearGameState.h"
#include "GameFramework/GearGameMode.h"
#include "GameFramework/GearBuilderPawn.h"
#include "Vehicle/VehicleCamera.h"
#include "GearHUD.h"
#include "Placeable/GearPlaceable.h"
#include "Placeable/HazardSocketMarker.h"
#include "Placeable/HazardSocketComponent.h"
#include "GameSystems/TrackSpline.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

AGearPlayerController::AGearPlayerController()
{
	bAutoManageActiveCameraTarget = false;

	IsReady = false;

	bDraggingTouch_1 = false;
	bDraggingTouch_2 = false;
	LastTouch_1 = FVector2D::ZeroVector;
	LastTouch_2 = FVector2D::ZeroVector;
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
		UpdatePinchValueAndInjectInput();
		UpdateScreenDragValueAndInjectInput();
		
	}
}

void AGearPlayerController::UpdateScreenDragValueAndInjectInput()
{
	bool bKeyDown = false;
	FVector2D KeyPosition = FVector2D(0.0f);

#if PLATFORM_WINDOWS

	bKeyDown = IsInputKeyDown(EKeys::LeftMouseButton);
	GetMousePosition(KeyPosition.X, KeyPosition.Y);

#elif PLATFORM_ANDROID

	GetInputTouchState(ETouchIndex::Touch1, KeyPosition.X, KeyPosition.Y, bKeyDown);

#endif

	FVector2D ScreenDragValue;

	if (!bKeyDown)
	{
		ScreenDragValue = FVector2D::Zero();
		LastTouch_1 = FVector2D::Zero();
		bDraggingTouch_1 = false;
	}

	// just started dragging
	else if (bKeyDown && !bDraggingTouch_1)
	{
		ScreenDragValue = FVector2D::Zero();
		LastTouch_1 = KeyPosition;
		bDraggingTouch_1 = true;
	}

	else
	{
		ScreenDragValue = KeyPosition - LastTouch_1;
		LastTouch_1 = KeyPosition;
		bDraggingTouch_1 = true;
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

void AGearPlayerController::UpdatePinchValueAndInjectInput()
{
	float PinchValue = 0.0f;

#if PLATFORM_WINDOWS

	FVector2D KeyPosition;
	bool bKeyDown = IsInputKeyDown(EKeys::RightMouseButton);
	GetMousePosition(KeyPosition.X, KeyPosition.Y);

	if (!bKeyDown)
	{
		LastTouch_2 = FVector2D::ZeroVector;
		bDraggingTouch_2 = false;
	}

	else if (bKeyDown && !bDraggingTouch_2)
	{
		LastTouch_2 = KeyPosition;
		bDraggingTouch_2 = true;
	}

	else
	{
		PinchValue = KeyPosition.X - LastTouch_2.X;
		LastTouch_2 = KeyPosition;
	}


#elif PLATFORM_ANDROID

#endif

	if (UEnhancedInputLocalPlayerSubsystem* InputSystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		if (IsValid(PinchAction))
		{
			TArray<UInputModifier*> Modifiers;
			TArray<UInputTrigger*> Triggers;

			InputSystem->InjectInputForAction(PinchAction, FInputActionValue(FInputActionValue::Axis1D(PinchValue)), Modifiers, Triggers);
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

void AGearPlayerController::ClientStateAllPlayersJoined_End()
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
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

void AGearPlayerController::PlaceHazard_Implementation(class UHazardSocketComponent* TargetSocket)
{
	AGearGameMode* GearGameMode = Cast<AGearGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(GearGameMode))
	{
		GearGameMode->RequestPlaceHazardForPlayer(this, TargetSocket);
	}
}

void AGearPlayerController::ClickedOnHazardSocketMarker(class AHazardSocketMarker* HazardSocketMarker)
{
	if (IsValid(HazardSocketMarker) && IsValid(HazardSocketMarker->TargetSocket))
	{
		PlaceHazard(HazardSocketMarker->TargetSocket);
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

void AGearPlayerController::NotifyFurthestReachedCheckpoint(int32 FurthestReachedCheckpoint, int32 CheckpointsNum, float ReachTime)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->NotifyFurthestReachedCheckpoint(FurthestReachedCheckpoint, CheckpointsNum, ReachTime);
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