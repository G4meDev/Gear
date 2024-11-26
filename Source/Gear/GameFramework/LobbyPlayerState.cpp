// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/LobbyPlayerState.h"
#include "GameFramework/LobbyplayerController.h"
#include "GameFramework/LobbyGameState.h"
#include "Utils/DataHelperBFL.h"
#include "Net/UnrealNetwork.h"

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPlayerState, ColorCode);
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

	OnPlayerNameChanged.Broadcast();
}

void ALobbyPlayerState::OnRep_ColorCode(EPlayerColorCode OldColor)
{
	PlayerColor = UDataHelperBFL::ResolveColorCode(ColorCode);
	

}

void ALobbyPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	// after travel player name just get set locally and server wont notice the name change

	ENetMode NetMode = GetNetMode();
	if (NetMode == NM_ListenServer || NetMode == NM_Standalone)
	{
		PlayerState->OnRep_PlayerName();
	}


}
