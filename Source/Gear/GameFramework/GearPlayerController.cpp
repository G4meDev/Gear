// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearGameState.h"
#include "GameFramework/GearGameMode.h"
#include "GameFramework/GearBuilderPawn.h"
#include "GearHUD.h"
#include "Hazard/GearHazardActor.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

AGearPlayerController::AGearPlayerController()
{
	IsReady = false;
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

void AGearPlayerController::ClientStateAllPlayersJoined_Implementation()
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->AllPlayersJoined();
	}
}

void AGearPlayerController::ClientStateMatchStarted_Implementation()
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->RemoveWaitingScreen();
	}
}

void AGearPlayerController::SelectHazard_Implementation(AGearHazardActor* Hazard)
{
	AGearPlayerState* GearPlayer = GetPlayerState<AGearPlayerState>();

	if (IsValid(GearPlayer))
	{
		AGearGameMode* GearGameMode = Cast<AGearGameMode>(GetWorld()->GetAuthGameMode());

		if (IsValid(GearGameMode))
		{
			GearGameMode->RequestSelectingHazardForPlayer(Hazard, GearPlayer);
		}
	}
}

void AGearPlayerController::ClientStateSelectingPieces_Implementation(float StateStartTime)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->StartSelectingPieces(StateStartTime);
	}
}

void AGearPlayerController::ClientStatePlacingPieces_Implementation(float StateStartTime)
{
	AGearHUD* GearHUD = GetHUD<AGearHUD>();
	if (IsValid(GearHUD))
	{
		GearHUD->StartPlacingPieces(StateStartTime);
	}

	AGearBuilderPawn* BuilderPawn = GetPawn<AGearBuilderPawn>();
	if (BuilderPawn)
	{
		BuilderPawn->StartPlacing();
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