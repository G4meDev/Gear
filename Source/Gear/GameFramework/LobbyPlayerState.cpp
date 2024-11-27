// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/LobbyPlayerState.h"
#include "GameFramework/LobbyplayerController.h"
#include "GameFramework/LobbyGameState.h"
#include "GameFramework/GearPlayerState.h"
#include "Utils/DataHelperBFL.h"
#include "Net/UnrealNetwork.h"

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPlayerState, ColorCode);
}

ALobbyPlayerState::ALobbyPlayerState()
{
	ColorCode = EPlayerColorCode::Black;
	OnRep_ColorCode();
}

void ALobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();


}

void ALobbyPlayerState::Destroyed()
{
	Super::Destroyed();


}

void ALobbyPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	
}

void ALobbyPlayerState::OnRep_ColorCode()
{
	PlayerColor = UDataHelperBFL::ResolveColorCode(ColorCode);
	

}

void ALobbyPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	AGearPlayerState* GearPlayerState = Cast<AGearPlayerState>(PlayerState);
	if (IsValid(GearPlayerState))
	{
		GearPlayerState->ColorCode = ColorCode;
		GearPlayerState->OnRep_ColorCode();
	}
}