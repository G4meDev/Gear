// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearPlayerState.h"

void AGearPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	OnPlayerNameChanged.Broadcast();
}

void AGearPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);


}

void AGearPlayerState::OnRep_ColorCode()
{
	

}
