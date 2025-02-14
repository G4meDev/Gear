// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/LobbyPlayerController.h"
#include "GameFramework/GearGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/LobbyPlayerState.h"
#include "GameFramework/LobbyGameState.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

ALobbyPlayerController::ALobbyPlayerController()
{
// 	LastDummyPacketTime = FLT_MAX;
// 	DummyPacketInterval = 1.0f;
// 	TimeoutLimit = 15.0f;
	
	DisconnectionReason = EPlayerDisconnectionReason::ConnectionFailure;
}

void ALobbyPlayerController::PostNetInit()
{
	Super::PostNetInit();


}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
// 		if (!HasAuthority() && GetWorld())
// 		{
// 			GetWorld()->GetTimerManager().SetTimer(DummtPacketTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::SendDummyPacket_Server), DummyPacketInterval, true);
// 		}

		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IsValid(InputMappingContext))
			{
				InputSystem->AddMappingContext(InputMappingContext, 0);
			}
		}

		UGearGameInstance* GearGameInstance = GetGameInstance<UGearGameInstance>();
		if (IsValid(GearGameInstance))
		{
			GearGameInstance->SetDisconnectionReason(DisconnectionReason);
		}

#if PLATFORM_WINDOWS

		LeftMouseInputHandler.Init(EKeys::LeftMouseButton);
		RightMouseInputHandler.Init(EKeys::RightMouseButton);

#elif PLATFORM_ANDROID

		Touch1_InputHandler.Init(ETouchIndex::Touch1);
		Touch2_InputHandler.Init(ETouchIndex::Touch2);

#endif

		UGearGameInstance* GearGameIntance = Cast<UGearGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		Server_SetPlayerName(GearGameIntance->GetPlayerName());

		if (HasAuthority())
		{
			AddLobbyMenu();
		}
	}

}

void ALobbyPlayerController::Destroyed()
{
	UGearGameInstance* GearGameInstance = GetGameInstance<UGearGameInstance>();
	if (IsValid(GearGameInstance))
	{
		GearGameInstance->SetDisconnectionReason(DisconnectionReason);
	}

	Super::Destroyed();
}

void ALobbyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (IsLocalController())
	{
		AddLobbyMenu();
	}
}

void ALobbyPlayerController::AddLobbyMenu()
{
	if (IsValid(LobbyWidget))
	{
		return;
	}

	LobbyWidget = CreateWidget<UUserWidget>(GetWorld(), LobbyWidgetClass);
	if (IsValid(LobbyWidget))
	{
		LobbyWidget->AddToViewport();
	}

	SetShowMouseCursor(true);
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, nullptr, EMouseLockMode::DoNotLock, false);
}

void ALobbyPlayerController::Server_SetPlayerName_Implementation(const FString& PlayerName)
{
	PlayerState->SetPlayerName(PlayerName);
}

// void ALobbyPlayerController::SendDummyPacket_Server_Implementation()
// {
// 	LastDummyPacketTime = GetWorld()->GetTimeSeconds();
// }

void ALobbyPlayerController::NotifyKicked_Client_Implementation()
{
	DisconnectionReason = EPlayerDisconnectionReason::Kicked;
}

void ALobbyPlayerController::QuitLobby()
{
	DisconnectionReason = EPlayerDisconnectionReason::Quit;
	ConsoleCommand("disconnect");
}

void ALobbyPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

// 	if (HasAuthority() && !IsLocalController())
// 	{
// 		if (LastDummyPacketTime < GetWorld()->GetTimeSeconds() - TimeoutLimit)
// 		{
// 			DisconnectionReason = EPlayerDisconnectionReason::ConnectionFailure;
// 			Destroy();
// 		}
// 	}

	if (IsLocalController())
	{
		FVector2D ScreenDragValue = FVector2D::ZeroVector;
		float PinchValue = 0.0f;
		bool bTouchRelease;

#if PLATFORM_WINDOWS

		LeftMouseInputHandler.Tick(this);
		RightMouseInputHandler.Tick(this);

		ScreenDragValue = LeftMouseInputHandler.GetVelocity();
		PinchValue = RightMouseInputHandler.GetVelocity().X;

		bTouchRelease = LeftMouseInputHandler.Released();

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

		bTouchRelease = Touch1_InputHandler.Released();

#endif

		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = GetLocalPlayer() ? GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr)
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

			if (IsValid(TouchEndAction))
			{
				InputSystem->InjectInputForAction(TouchEndAction, FInputActionValue(FInputActionValue::Axis1D(bTouchRelease)), Modifiers, Triggers);
			}
		}
	}
}

void ALobbyPlayerController::NotifyNewPlayer(APlayerState* InPlayer)
{
	ALobbyPlayerState* LobbyPlayer = Cast<ALobbyPlayerState>(InPlayer);
	if (IsValid(LobbyPlayer))
	{
		OnAddPlayer(LobbyPlayer);
	}
}

void ALobbyPlayerController::NotifyRemovePlayer(APlayerState* InPlayer)
{
	ALobbyPlayerState* LobbyPlayer = Cast<ALobbyPlayerState>(InPlayer);
	if (IsValid(LobbyPlayer))
	{
		OnRemovePlayer(LobbyPlayer);
	}	
}

void ALobbyPlayerController::RequestColorChange_Implementation(EPlayerColorCode Color)
{
	ALobbyGameState* LobbyGameState = Cast<ALobbyGameState>(GetWorld()->GetGameState());

	if (IsValid(LobbyGameState))
	{
		LobbyGameState->RequestColorChangeForPlayer(this, Color);
	}
}

void ALobbyPlayerController::ColorChange_RollBack_Implementation(EPlayerColorCode Color)
{
	ALobbyPlayerState* LobbyPlayer = GetLobbyPlayerState();
	if (IsValid(LobbyPlayer))
	{
		LobbyPlayer->ColorCode = Color;
		LobbyPlayer->OnRep_ColorCode();
	}
}

void ALobbyPlayerController::TryChangeColor(EPlayerColorCode Color)
{
	// change color locally. if color is invalid then server sends a roll back rpc
	if (!HasAuthority())
	{
		ALobbyPlayerState* LobbyPlayer = GetLobbyPlayerState();
		if (IsValid(LobbyPlayer))
		{
			LobbyPlayer->ColorCode = Color;
			LobbyPlayer->OnRep_ColorCode();
		}
	}

	RequestColorChange(Color);
}

ALobbyPlayerState* ALobbyPlayerController::GetLobbyPlayerState()
{
	return Cast<ALobbyPlayerState>(PlayerState);
}
