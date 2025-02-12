// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/LobbyGameMode.h"
#include "GameFramework/LobbyPlayerController.h"
#include "GameFramework/LobbyGameState.h"
#include "GameFramework/LobbyPlayerState.h"
#include "GameFramework/GameSession.h"
#include "Net/OnlineEngineInterface.h"

ALobbyGameMode::ALobbyGameMode()
{
	PlayerControllerClass = ALobbyPlayerController::StaticClass();
	GameStateClass = ALobbyGameState::StaticClass();
	PlayerStateClass = ALobbyPlayerState::StaticClass();

	bUseSeamlessTravel = true;

}

void ALobbyGameMode::KickPlayer(class ALobbyPlayerController* Player)
{
	if (IsValid(Player) && IsValid(Cast<UNetConnection>(Player->Player)))
	{
		if (IsValid(Player->GetPawn()))
		{
			Player->GetPawn()->Destroy();
		}

		Player->NotifyKicked_Client();
		Player->Destroy();
	}
}

void ALobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	const bool bUniqueIdCheckOk = (!UniqueId.IsValid() || UOnlineEngineInterface::Get()->IsCompatibleUniqueNetId(UniqueId));
	if (!bUniqueIdCheckOk)
	{
		ErrorMessage = TEXT("incompatible_unique_net_id");
	}

	else if (!HasEmptySlot())
	{
		ErrorMessage = FPlayerDisconnectionStrings::FullLobbyString;
	}

	else if (IsGameStarting())
	{
		ErrorMessage = FPlayerDisconnectionStrings::TryingToStartGame;
	}

	else
	{
		ErrorMessage = GameSession->ApproveLogin(Options);
	}

	FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage);
}

bool ALobbyGameMode::HasEmptySlot()
{
	ALobbyGameState* LobbyGameState = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
	if (IsValid(LobbyGameState) && LobbyGameState->NumAllowedPlayers > GetNumPlayers())
	{
		return true;
	}

	return false;
}

bool ALobbyGameMode::IsGameStarting()
{
	ALobbyGameState* LobbyGameState = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
	if (IsValid(LobbyGameState))
	{
		return LobbyGameState->LobbyGameState == ELobbyGameState::StartingGame;
	}

	return true;
}