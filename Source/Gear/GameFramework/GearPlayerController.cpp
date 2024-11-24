// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearGameState.h"
#include "GearHUD.h"

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

	else
	{
		GearHUD = nullptr;
	}
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