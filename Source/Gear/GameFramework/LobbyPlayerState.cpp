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
	DOREPLIFETIME(ALobbyPlayerState, bReady);
}

ALobbyPlayerState::ALobbyPlayerState()
{
	ColorCode = EPlayerColorCode::Black;
	OnRep_ColorCode();

	PlayerJoinTime = 0.0f;
}

void ALobbyPlayerState::PostNetInit()
{
	Super::PostNetInit();

}

void ALobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		PlayerJoinTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

		FPlayerCustomization OldCustomization = PlayerCusstomization;
		PlayerCusstomization = FPlayerCustomization::GetRandomCustomization();
		OnRep_PlayerCustomization(PlayerCusstomization);
	}
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

void ALobbyPlayerState::OnRep_PlayerJoinTime()
{
	ALobbyGameState* GameState = GetWorld()->GetGameState<ALobbyGameState>();
	if (IsValid(GameState))
	{
		GameState->ReconstructPlayersPlatform();
	}
}

FPlayerCustomization ALobbyPlayerState::GetPlayerCustomization()
{
	return PlayerCusstomization;
}

void ALobbyPlayerState::SetPlayerCustomization_Server_Implementation(const FPlayerCustomization& InPlayerCustomization)
{
	FPlayerCustomization OldCustomization = PlayerCusstomization;
	PlayerCusstomization = InPlayerCustomization;
	OnRep_PlayerCustomization(OldCustomization);
}

void ALobbyPlayerState::SetPlayerHeadType_Server_Implementation(EDriverHead HeadType)
{
	FPlayerCustomization OldCustomization = PlayerCusstomization;
	PlayerCusstomization.HeadType.Type = HeadType;
	OnRep_PlayerCustomization(OldCustomization);
}

void ALobbyPlayerState::SetPlayerTricycleColor_Server_Implementation(EPlayerColorCode Color)
{
	FPlayerCustomization OldCustomization = PlayerCusstomization;
	PlayerCusstomization.TricycleColor.ColorCode = Color;
	OnRep_PlayerCustomization(OldCustomization);
}

void ALobbyPlayerState::SetPlayerClothColor_Server_Implementation(EPlayerColorCode Color)
{
	FPlayerCustomization OldCustomization = PlayerCusstomization;
	PlayerCusstomization.ClothColor.ColorCode = Color;
	OnRep_PlayerCustomization(OldCustomization);
}

void ALobbyPlayerState::SetPlayerPantColor_Server_Implementation(EPlayerColorCode Color)
{
	FPlayerCustomization OldCustomization = PlayerCusstomization;
	PlayerCusstomization.PantColor.ColorCode = Color;
	OnRep_PlayerCustomization(OldCustomization);
}

void ALobbyPlayerState::SetPlayerHandColor_Server_Implementation(EPlayerColorCode Color)
{
	FPlayerCustomization OldCustomization = PlayerCusstomization;
	PlayerCusstomization.HandColor.ColorCode = Color;
	OnRep_PlayerCustomization(OldCustomization);
}

void ALobbyPlayerState::SetPlayerShoeColor_Server_Implementation(EPlayerColorCode Color)
{
	FPlayerCustomization OldCustomization = PlayerCusstomization;
	PlayerCusstomization.ShoeColor.ColorCode = Color;
	OnRep_PlayerCustomization(OldCustomization);
}

void ALobbyPlayerState::SetReady_Server_Implementation(bool InbReady)
{
	bReady = InbReady;
}

bool ALobbyPlayerState::IsReady()
{
	return bReady;
}

float ALobbyPlayerState::GetPlayerJoinTime() const
{
	return PlayerJoinTime;
}

bool ALobbyPlayerState::SortJoinTimePredicate(const ALobbyPlayerState& P1, const ALobbyPlayerState& P2)
{
	return P1.GetPlayerJoinTime() < P2.GetPlayerJoinTime();
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

void ALobbyPlayerState::OnRep_PlayerCustomization(FPlayerCustomization OldCustomization)
{
	PlayerCusstomization.ResolveData();

	if (OldCustomization.HeadType.Type != PlayerCusstomization.HeadType.Type)
	{
		OnPlayerCustomizationHeadChanged.Broadcast();
	}

	OnPlayerCustomizationColorChanged.Broadcast();
}