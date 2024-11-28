// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearGameState.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearPlayerController.h"
#include "Net/UnrealNetwork.h"

AGearGameState::AGearGameState()
{

}

// void AGearGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
// 	
// }

void AGearGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			AGearPlayerController* GearController = Cast<AGearPlayerController>(PlayerController);

			if (IsValid(GearController))
			{
				AGearPlayerState* GearPlayer = Cast<AGearPlayerState>(PlayerState);
				if (IsValid(GearPlayer))
				{
					GearController->OnNewPlayer(GearPlayer);
				}
			}
		}
	}
}

void AGearGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			AGearPlayerController* GearController = Cast<AGearPlayerController>(PlayerController);

			if (IsValid(GearController))
			{
				AGearPlayerState* GearPlayer = Cast<AGearPlayerState>(PlayerState);
				if (IsValid(GearPlayer))
				{
					GearController->OnRemovePlayer(GearPlayer);
				}
			}
		}
	}
}
