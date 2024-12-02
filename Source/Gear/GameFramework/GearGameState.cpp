// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearGameState.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearPlayerController.h"

#include "Placeable/GearRoadModule.h"
#include "Placeable/GearHazard.h"
#include "Placeable/PlaceableSocket.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AGearGameState::AGearGameState()
{
	GearMatchState = EGearMatchState::WaitingForPlayerToJoin;

}

void AGearGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AGearGameState, GearMatchState);
	DOREPLIFETIME(AGearGameState, RoadModuleStack);
}

void AGearGameState::BeginPlay()
{
	Super::BeginPlay();


}

void AGearGameState::OnRep_GearMatchState(EGearMatchState OldState)
{
	switch (GearMatchState)
	{
	case EGearMatchState::WaitingForPlayerToJoin:
		break;

	case EGearMatchState::AllPlayersJoined:
		AllPlayerJoined_Start();
		break;

	case EGearMatchState::SelectingPlaceables:
		SelectingPlaceables_Start();
		break;

	case EGearMatchState::Placing:
		Placing_Start();
		break;

	case EGearMatchState::Racing:
		break;

	case EGearMatchState::Ended:
		break;

	default:
		break;
	}
}

void AGearGameState::OnRep_RoadModuleStack()
{

}

void AGearGameState::AllPlayerJoined_Start()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AGearPlayerController* GearController = Cast<AGearPlayerController>(*Iterator);
		if (IsValid(GearController) && GearController->IsLocalController())
		{
			GearController->ClientStateAllPlayersJoined();
		}
	}
}

void AGearGameState::SelectingPlaceables_Start()
{
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AGearPlayerController* PlayerController = Cast<AGearPlayerController>(*It);
		if (IsValid(PlayerController) && PlayerController->IsLocalController())
		{
			PlayerController->ClientStateMatchStarted();
			PlayerController->ClientStateSelectingPieces(GetWorld()->GetTimeSeconds());
		}
	}
}

void AGearGameState::Placing_Start()
{
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AGearPlayerController* PlayerController = Cast<AGearPlayerController>(*It);
		if (IsValid(PlayerController) && PlayerController->IsLocalController())
		{
			PlayerController->ClientStatePlacing(GetWorld()->GetTimeSeconds());
		}
	}
}

bool AGearGameState::FindStartRoadModuleAndAddToStack()
{
	TArray<AActor*> AlreadyPlacedRoadModules;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGearRoadModule::StaticClass(), AlreadyPlacedRoadModules);

	if (AlreadyPlacedRoadModules.Num() != 1)
	{
		UE_LOG(LogTemp, Error, TEXT("only one road module should be placed in level but found %i"), AlreadyPlacedRoadModules.Num());
		return false;
	}

	AGearRoadModule* Module = Cast<AGearRoadModule>(AlreadyPlacedRoadModules[0]);
	RoadModuleStack.Add(Module);
	return true;
}

UPlaceableSocket* AGearGameState::GetRoadEndSocket()
{
	if (!RoadModuleStack.IsEmpty())
	{
		return RoadModuleStack.Top()->RoadEndSocket;
	}

	return nullptr;
}

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