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
}

void AGearPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AGearPlayerController::PostNetInit()
{
	Super::PostNetInit();

	if (!HasAuthority())
	{
		RequestWorldTime_Internal();
		if (NetworkClockUpdateFrequency > 0.f)
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::RequestWorldTime_Internal, NetworkClockUpdateFrequency, true);
		}
	}
}

void AGearPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GearGameState = GetWorld()->GetGameState<AGearGameState>();

	if (IsLocalController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IsValid(InputMappingContext))
			{
				InputSystem->AddMappingContext(InputMappingContext, 0);
			}
		}

#if PLATFORM_WINDOWS

		LeftMouseInputHandler.Init(EKeys::LeftMouseButton);
		RightMouseInputHandler.Init(EKeys::RightMouseButton);

#elif PLATFORM_ANDROID

		Touch1_InputHandler.Init(ETouchIndex::Touch1);
		Touch2_InputHandler.Init(ETouchIndex::Touch2);

#endif
	}


}

void AGearPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocalController())
	{
		UpdateAndInjectInputs();
	}
}

void AGearPlayerController::UpdateAndInjectInputs()
{
	FVector2D ScreenDragValue = FVector2D::ZeroVector;
	float PinchValue = 0.0f;

#if PLATFORM_WINDOWS

	LeftMouseInputHandler.Tick(this);
	RightMouseInputHandler.Tick(this);

	ScreenDragValue = LeftMouseInputHandler.GetVelocity();
	PinchValue = RightMouseInputHandler.GetVelocity().X;

#elif PLATFORM_ANDROID

	Touch1_InputHandler.Tick(this);
	Touch2_InputHandler.Tick(this);

	if (Touch1_InputHandler.IsHoldingTouch() && Touch2_InputHandler.IsHoldingTouch())
	{
		FVector2D Center = (Touch1_InputHandler.GetValue() + Touch2_InputHandler.GetValue()) / 2;

		auto CalculateVelocityAlongCentroid = [&](const FTouchInputHandler& InputHandler)
			{
				FVector2D CentroidDir = Center - InputHandler.GetValue();
				CentroidDir.Normalize();
				return FVector2D::DotProduct(CentroidDir, InputHandler.GetVelocity());
			};

		PinchValue = CalculateVelocityAlongCentroid(Touch1_InputHandler) + CalculateVelocityAlongCentroid(Touch2_InputHandler);
		PinchValue = -PinchValue;
	}

	else
	{
		ScreenDragValue = Touch1_InputHandler.IsHoldingTouch() ? Touch1_InputHandler.GetVelocity() : Touch2_InputHandler.GetVelocity();
	}

#endif

	if (UEnhancedInputLocalPlayerSubsystem* InputSystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		TArray<UInputModifier*> Modifiers;
		TArray<UInputTrigger*> Triggers;

		if (IsValid(MoveScreenAction))
		{
			InputSystem->InjectInputForAction(MoveScreenAction, FInputActionValue(FInputActionValue::Axis2D(ScreenDragValue)), Modifiers, Triggers);
		}

		if (IsValid(PinchAction))
		{
			InputSystem->InjectInputForAction(PinchAction, FInputActionValue(FInputActionValue::Axis1D(PinchValue)), Modifiers, Triggers);
		}
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

void AGearPlayerController::ClientStateWaiting_Start(float StartTime)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->Waiting_Start(StartTime);
	}
}

void AGearPlayerController::ClientStateWaiting_End()
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->Waiting_End();
	}
}

void AGearPlayerController::ClientStateSelecting_Start(float StartTime)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->Selecting_Start(StartTime);
	}
}

void AGearPlayerController::ClientStateSelecting_End()
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->Selecting_End();
	}
}

void AGearPlayerController::ClientStatePlacing_Start(float StartTime)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->Placing_Start(StartTime);
	}
}

void AGearPlayerController::ClientStatePlacing_End()
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->Placing_End();
	}
}

void AGearPlayerController::ClientStateRacing_Start(float StartTime)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->Racing_Start(StartTime);
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

void AGearPlayerController::ClientStateScoreboard_Start(float StartTime, const TArray<FCheckpointResult>& RoundResults)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		//GearHUD->Scoreboard_Start(StateStartTime, RoundResults);
		GearHUD->Scoreboard_Start(StartTime);
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

void AGearPlayerController::ClientStateFinishboard_Start(float StartTime)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->Finishboard_Start(StartTime);
	}
}

void AGearPlayerController::ClientStateFinishboard_End()
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->Finishboard_End();
	}
}

void AGearPlayerController::NotifyFurthestReachedCheckpoint(int32 FurthestReachedCheckpoint, int32 CheckpointsNum, float ReachTime)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->ReachedNewCheckpoint(FurthestReachedCheckpoint, CheckpointsNum, ReachTime);
	}
}

void AGearPlayerController::NotifyAllPlayerJoined()
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->AllPlayersJoined();
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

void AGearPlayerController::OnPlayerJoined(AGearPlayerState* GearPlayer)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->PlayerJoined(GearPlayer);
	}
}

void AGearPlayerController::OnPlayerQuit(AGearPlayerState* GearPlayer)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->PlayerQuit(GearPlayer);
	}
}

// ------------------------------------------------------------------------------------------

void AGearPlayerController::RequestWorldTime_Internal()
{
	ServerRequestWorldTime(GetWorld()->GetTimeSeconds());
}

void AGearPlayerController::ServerRequestWorldTime_Implementation(float ClientTimestamp)
{
	const float Timestamp = GetWorld()->GetTimeSeconds();
	ClientUpdateWorldTime(ClientTimestamp, Timestamp);
}

void AGearPlayerController::ClientUpdateWorldTime_Implementation(float ClientTimestamp, float ServerTimestamp)
{
	const float RoundTripTime = GetWorld()->GetTimeSeconds() - ClientTimestamp;
	RTTCircularBuffer.Add(RoundTripTime);
	float AdjustedRTT = 0;
	if (RTTCircularBuffer.Num() == 10)
	{
		TArray<float> tmp = RTTCircularBuffer;
		tmp.Sort();
		for (int i = 1; i < 9; ++i)
		{
			AdjustedRTT += tmp[i];
		}
		AdjustedRTT /= 8;
		RTTCircularBuffer.RemoveAt(0);
	}
	else
	{
		AdjustedRTT = RoundTripTime;
	}

	float ServerWorldTimeDelta = ServerTimestamp - ClientTimestamp - AdjustedRTT / 2.f;

	if (IsValid(GearGameState))
	{
		GearGameState->SetServerTimeDelay(ServerWorldTimeDelta);
	}
}