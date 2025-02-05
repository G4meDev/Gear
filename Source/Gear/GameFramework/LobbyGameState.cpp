// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/LobbyGameState.h"
#include "GameFramework/LobbyPlayerController.h"
#include "GameFramework/LobbyPlayerState.h"
#include "GameSystems/LobbyPlayerPlatform.h"
#include "Kismet/GameplayStatics.h"

ALobbyGameState::ALobbyGameState()
{

}

void ALobbyGameState::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> PlatformActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyPlayerPlatform::StaticClass(), PlatformActors);

	PlayerPlatforms.Reserve(4);
	for (AActor* PlatformActor : PlatformActors)
	{
		ALobbyPlayerPlatform* PlayerPlatform = Cast<ALobbyPlayerPlatform>(PlatformActor);
		if (IsValid(PlayerPlatform))
		{
			PlayerPlatforms.Add(PlayerPlatform);
		}
	}

	for (APlayerState* PlayerState : PlayerArray)
	{
		ALobbyPlayerState* LobbyPlayerState = Cast<ALobbyPlayerState>(PlayerState);
		ConstructPlayerPlatform(LobbyPlayerState);
	}
}

void ALobbyGameState::GetInUseColors(TArray<EPlayerColorCode>& InUseColors)
{
	for (APlayerState* Player : PlayerArray)
	{
		ALobbyPlayerState* LobbyPlayer = Cast<ALobbyPlayerState>(Player);
		if (IsValid(LobbyPlayer))
		{
			InUseColors.Add(LobbyPlayer->ColorCode);
		}
	}
}

void ALobbyGameState::AssignNewColorToPlayer(APlayerState* Player)
{
	ALobbyPlayerState* LobbyPlayer = Cast<ALobbyPlayerState>(Player);
	if (IsValid(LobbyPlayer))
	{
		TArray<EPlayerColorCode> InUseColors;
		GetInUseColors(InUseColors);
		InUseColors.RemoveSingle(LobbyPlayer->ColorCode);

		for (uint8 i = 0; i < static_cast<uint8>(EPlayerColorCode::MAX_COLOR); i++)
		{
			EPlayerColorCode ColorCode = static_cast<EPlayerColorCode>(i);

			if (!InUseColors.Contains(ColorCode))
			{
				LobbyPlayer->ColorCode = ColorCode;
				LobbyPlayer->OnRep_ColorCode();
				return;
			}
		}
	}
}

void ALobbyGameState::RequestColorChangeForPlayer(ALobbyPlayerController* PC, EPlayerColorCode Color)
{
	ALobbyPlayerState* Player = Cast<ALobbyPlayerState>(PC->PlayerState);
	if (!IsValid(Player))
	{
		return;
	}

	TArray<EPlayerColorCode> InUseColors;
	GetInUseColors(InUseColors);

	if (!InUseColors.Contains(Color))
	{
		Player->ColorCode = Color;
		Player->OnRep_ColorCode();
	}

	else
	{
		PC->ColorChange_RollBack(Player->ColorCode);
	}
}

void ALobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	ALobbyPlayerState* LobbyPlayerState = Cast<ALobbyPlayerState>(PlayerState);
	if (IsValid(LobbyPlayerState))
	{
		LobbyPlayerState->PlayerJoinTime = GetServerWorldTimeSeconds();
		ConstructPlayerPlatform(LobbyPlayerState);
	}

	if (HasAuthority())
	{
		AssignNewColorToPlayer(PlayerState);
	}


	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			ALobbyPlayerController* LobbyController = Cast<ALobbyPlayerController>(PlayerController);

			if (IsValid(LobbyController))
			{
				LobbyController->NotifyNewPlayer(PlayerState);
			}
		}
	}
}

void ALobbyGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	ALobbyPlayerState* LobbyPlayerState = Cast<ALobbyPlayerState>(PlayerState);
	if (IsValid(LobbyPlayerState))
	{
		LobbyPlayerState->PlayerJoinTime = GetServerWorldTimeSeconds();
		DestructPlayerPlatform(LobbyPlayerState);
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			ALobbyPlayerController* LobbyController = Cast<ALobbyPlayerController>(PlayerController);

			if (IsValid(LobbyController))
			{
				LobbyController->NotifyRemovePlayer(PlayerState);
			}
		}
	}
}

void ALobbyGameState::ConstructPlayerPlatform(class ALobbyPlayerState* PlayerState)
{
	for (ALobbyPlayerPlatform* PlayerPlatform : PlayerPlatforms)
	{
		if (!IsValid(PlayerPlatform->GetOwningPlayer()))
		{
			PlayerPlatform->SetOwningPlayer(PlayerState);
			return;
		}
	}
}

void ALobbyGameState::DestructPlayerPlatform(class ALobbyPlayerState* PlayerState)
{
	for (ALobbyPlayerPlatform* PlayerPlatform : PlayerPlatforms)
	{
		if (IsValid(PlayerPlatform->GetOwningPlayer()) && PlayerPlatform->GetOwningPlayer() == PlayerState)
		{
			PlayerPlatform->SetOwningPlayer(nullptr);
			return;
		}
	}
}