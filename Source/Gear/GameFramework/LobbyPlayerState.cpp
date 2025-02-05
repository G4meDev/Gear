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
	DOREPLIFETIME(ALobbyPlayerState, PlayerJoinTime);
	DOREPLIFETIME(ALobbyPlayerState, PlayerCusstomization);
}

ALobbyPlayerState::ALobbyPlayerState()
{
	ColorCode = EPlayerColorCode::Black;
	OnRep_ColorCode();

	PlayerJoinTime = 0.0f;
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

FPlayerCustomization ALobbyPlayerState::GetPlayerCustomization()
{
	return PlayerCusstomization;
}

void ALobbyPlayerState::SetPlayerCustomization_Server_Implementation(const FPlayerCustomization& InPlayerCustomization)
{
	PlayerCusstomization = InPlayerCustomization;
	OnRep_PlayerCustomization();
}

void ALobbyPlayerState::SetPlayerHeadType_Server_Implementation(EDriverHead HeadType)
{
	PlayerCusstomization.HeadType = HeadType;
	OnRep_PlayerCustomization();
}

void ALobbyPlayerState::SetPlayerTricycleColor_Server_Implementation(EPlayerColorCode Color)
{
	PlayerCusstomization.TricycleColor = Color;
	OnRep_PlayerCustomization();
}

void ALobbyPlayerState::SetPlayerClothColor_Server_Implementation(EPlayerColorCode Color)
{
	PlayerCusstomization.ClothColor = Color;
	OnRep_PlayerCustomization();
}

void ALobbyPlayerState::SetPlayerPantColor_Server_Implementation(EPlayerColorCode Color)
{
	PlayerCusstomization.PantColor = Color;
	OnRep_PlayerCustomization();
}

void ALobbyPlayerState::SetPlayerHandColor_Server_Implementation(EPlayerColorCode Color)
{
	PlayerCusstomization.HandColor = Color;
	OnRep_PlayerCustomization();
}

void ALobbyPlayerState::SetPlayerShoeColor_Server_Implementation(EPlayerColorCode Color)
{
	PlayerCusstomization.ShoeColor = Color;
	OnRep_PlayerCustomization();
}

float ALobbyPlayerState::GetPlayerJoinTime()
{
	return PlayerJoinTime;
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

void ALobbyPlayerState::OnRep_PlayerCustomization()
{
	OnPlayerCustomizationChanged.Broadcast();
}