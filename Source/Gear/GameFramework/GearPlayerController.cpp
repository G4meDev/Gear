// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearGameState.h"
#include "GameFramework/GearGameMode.h"
#include "GearHUD.h"
#include "Hazard/GearHazardActor.h"

AGearPlayerController::AGearPlayerController()
{
	GearHUD = nullptr;
	IsReady = false;
}

void AGearPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		GearHUD = Cast<AGearHUD>(GetHUD());
		GearHUD->ShowWaitingScreen();

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

void AGearPlayerController::AllPlayersJoined_Implementation()
{
	if (IsValid(GearHUD))
	{
		GearHUD->AllPlayersJoined();
	}
}

void AGearPlayerController::MatchStarted_Implementation()
{
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

void AGearPlayerController::PeekClientIsReady_Implementation()
{
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
	if(IsValid(GearHUD))
		GearHUD->AddPlayer(GearPlayer);
}

void AGearPlayerController::OnRemovePlayer(AGearPlayerState* GearPlayer)
{
	if (IsValid(GearHUD))
		GearHUD->RemovePlayer(GearPlayer);
}