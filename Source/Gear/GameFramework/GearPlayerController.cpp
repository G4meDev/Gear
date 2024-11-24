// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearPlayerState.h"

void AGearPlayerController::NotifyNewPlayer(APlayerState* InPlayer)
{
	AGearPlayerState* GearPlayer = Cast<AGearPlayerState>(InPlayer);
	if (IsValid(GearPlayer))
	{
		OnAddPlayer(GearPlayer);
	}
}

void AGearPlayerController::NotifyRemovePlayer(APlayerState* InPlayer)
{
	AGearPlayerState* GearPlayer = Cast<AGearPlayerState>(InPlayer);
	if (IsValid(GearPlayer))
	{
		OnRemovePlayer(GearPlayer);
	}
}