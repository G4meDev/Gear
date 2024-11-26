// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearPlayerState.h"
#include "Utils/DataHelperBFL.h"
#include "Net/UnrealNetwork.h"

AGearPlayerState::AGearPlayerState()
{
	ColorCode = EPlayerColorCode::Black;
	OnRep_ColorCode();
}

void AGearPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearPlayerState, ColorCode);
}

void AGearPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();


}

void AGearPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);


}


void AGearPlayerState::OnRep_ColorCode()
{
	PlayerColor = UDataHelperBFL::ResolveColorCode(ColorCode);

}
