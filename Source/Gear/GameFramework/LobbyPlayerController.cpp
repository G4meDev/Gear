// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/LobbyPlayerController.h"
#include "GameFramework/GearGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/LobbyPlayerState.h"
#include "GameFramework/LobbyGameState.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void ALobbyPlayerController::PostNetInit()
{
	Super::PostNetInit();


}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		UGearGameInstance* GearGameIntance = Cast<UGearGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		Server_SetPlayerName(GearGameIntance->GetPlayerName());

		if (HasAuthority())
		{
			AddLobbyMenu();
		}
	}

}

void ALobbyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (IsLocalController())
	{
		AddLobbyMenu();
	}
}

void ALobbyPlayerController::AddLobbyMenu()
{
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), LobbyWidgetClass);
	if (IsValid(Widget))
	{
		Widget->AddToViewport();
	}

	SetShowMouseCursor(true);
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this);
}

void ALobbyPlayerController::Server_SetPlayerName_Implementation(const FString& PlayerName)
{
	PlayerState->SetPlayerName(PlayerName);
}

void ALobbyPlayerController::NotifyNewPlayer(APlayerState* InPlayer)
{
	ALobbyPlayerState* LobbyPlayer = Cast<ALobbyPlayerState>(InPlayer);
	if (IsValid(LobbyPlayer))
	{
		OnAddPlayer(LobbyPlayer);
	}
}

void ALobbyPlayerController::NotifyRemovePlayer(APlayerState* InPlayer)
{
	ALobbyPlayerState* LobbyPlayer = Cast<ALobbyPlayerState>(InPlayer);
	if (IsValid(LobbyPlayer))
	{
		OnRemovePlayer(LobbyPlayer);
	}	
}

void ALobbyPlayerController::RequestColorChange_Implementation(EPlayerColorCode Color)
{
	ALobbyGameState* LobbyGameState = Cast<ALobbyGameState>(GetWorld()->GetGameState());

	if (IsValid(LobbyGameState))
	{
		LobbyGameState->RequestColorChangeForPlayer(this, Color);
	}
}

void ALobbyPlayerController::ColorChange_RollBack_Implementation(EPlayerColorCode Color)
{
	ALobbyPlayerState* LobbyPlayer = GetLobbyPlayerState();
	if (IsValid(LobbyPlayer))
	{
		LobbyPlayer->ColorCode = Color;
		LobbyPlayer->OnRep_ColorCode();
	}
}

void ALobbyPlayerController::TryChangeColor(EPlayerColorCode Color)
{
	// change color locally. if color is invalid then server sends a roll back rpc
	if (!HasAuthority())
	{
		ALobbyPlayerState* LobbyPlayer = GetLobbyPlayerState();
		if (IsValid(LobbyPlayer))
		{
			LobbyPlayer->ColorCode = Color;
			LobbyPlayer->OnRep_ColorCode();
		}
	}

	RequestColorChange(Color);
}

ALobbyPlayerState* ALobbyPlayerController::GetLobbyPlayerState()
{
	return Cast<ALobbyPlayerState>(PlayerState);
}
