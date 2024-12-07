// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearGameState.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearBuilderPawn.h"
#include "GameFramework/GearCameraManager.h"

#include "Placeable/GearRoadModule.h"
#include "Placeable/GearHazard.h"
#include "Placeable/PlaceableSocket.h"

#include "GameSystems/Checkpoint.h"
#include "GameSystems/TrackSpline.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AGearGameState::AGearGameState()
{
	GearMatchState = EGearMatchState::WaitingForPlayerToJoin;

}

void AGearGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AGearGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AGearGameState, GearMatchState);
	DOREPLIFETIME(AGearGameState, LastGameStateTransitionTime);
	DOREPLIFETIME(AGearGameState, RoadModuleStack);
	DOREPLIFETIME(AGearGameState, CheckpointsStack);
}

void AGearGameState::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(TrackSpline))
	{
		TrackSpline = GetWorld()->SpawnActor<ATrackSpline>(ATrackSpline::StaticClass(), FTransform::Identity);
		check(TrackSpline);
	}
}

void AGearGameState::Destroyed()
{
	Super::Destroyed();

	if (IsValid(TrackSpline))
	{
		TrackSpline->Destroy();
	}
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

	case EGearMatchState::Racing_WaitTime:
		Placing_End();
		Racing_Start();
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
	AGearRoadModule* TopModule = RoadModuleStack.Top();

	if (IsValid(TopModule) && TopModule->bShouldNotifyGameState && !TopModule->bGameStateNotified)
	{
		RoadModuleStack.Top()->bGameStateNotified = true;

		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (Iterator->Get()->IsLocalController())
			{
				AGearBuilderPawn* BuilderPawn = Iterator->Get()->GetPawn<AGearBuilderPawn>();
				if (IsValid(BuilderPawn))
				{
					BuilderPawn->RoadModuleStackChanged();
				}

				if (IsValid(TrackSpline))
				{
					TrackSpline->RoadModuleStackChanged(RoadModuleStack);
				}
			}
		}
	}
}

void AGearGameState::OnRep_CheckpointsStack()
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
			PlayerController->ClientStateSelectingPieces(LastGameStateTransitionTime);
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
			PlayerController->ClientStatePlacing(LastGameStateTransitionTime);
		}
	}
}

void AGearGameState::Placing_End()
{
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AGearPlayerController* PlayerController = Cast<AGearPlayerController>(*It);
		if (IsValid(PlayerController) && PlayerController->IsLocalController())
		{
			PlayerController->ClientStatePlacing_Finish();
		}
	}
}

void AGearGameState::Racing_Start()
{
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AGearPlayerController* PlayerController = Cast<AGearPlayerController>(*It);
		if (IsValid(PlayerController) && PlayerController->IsLocalController())
		{
			PlayerController->ClientStateRacing_Start(LastGameStateTransitionTime);
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

bool AGearGameState::FindStartCheckpointAndAddToStack()
{
	TArray<AActor*> AlreadyPlacedCheckpoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACheckpoint::StaticClass(), AlreadyPlacedCheckpoints);

	if (AlreadyPlacedCheckpoints.Num() != 1)
	{
		UE_LOG(LogTemp, Error, TEXT("only one checkpoint should be placed in level but found %i"), AlreadyPlacedCheckpoints.Num());
		return false;
	}

	ACheckpoint* Checkpoint = Cast<ACheckpoint>(AlreadyPlacedCheckpoints[0]);
	CheckpointsStack.Add(Checkpoint);
	return true;
}

UPlaceableSocket* AGearGameState::GetRoadStackAttachableSocket()
{
	if (!RoadModuleStack.IsEmpty() && IsValid(RoadModuleStack.Top()))
	{
		return RoadModuleStack.Top()->GetAttachableSocket();
	}
	
	return nullptr;
}

ACheckpoint* AGearGameState::GetCheckPointAtIndex(int Index)
{
	check(Index >= 0 && Index < CheckpointsStack.Num());
	
	return CheckpointsStack[Index];
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