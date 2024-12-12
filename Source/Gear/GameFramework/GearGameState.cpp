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

#include "Vehicle/GearVehicle.h"
#include "Vehicle/VehicleCamera.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AGearGameState::AGearGameState()
{
	GearMatchState = EGearMatchState::WaitingForPlayerToJoin;

	LastPlacedCheckpointModuleStackIndex = 0;
	FurthestReachedDistace = 0.0f;
	FurthestReachedCheckpoint = 0;
	RoundNumber = 1;
}

void AGearGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	for (AGearVehicle* Vehicle : Vehicles)
	{
		UpdateFurthestDistanceWithVehicle(Vehicle);
	}
}

void AGearGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AGearGameState, GearMatchState);
	DOREPLIFETIME(AGearGameState, LastGameStateTransitionTime);
	DOREPLIFETIME(AGearGameState, RoadModuleStack);
	DOREPLIFETIME(AGearGameState, CheckpointsStack);
	DOREPLIFETIME(AGearGameState, Vehicles);
	DOREPLIFETIME(AGearGameState, CheckpointResults);
	DOREPLIFETIME(AGearGameState, RoundNumber);
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

void AGearGameState::UpdateFurthestDistanceWithVehicle(AGearVehicle* GearVehicle)
{
	if (IsValid(GearVehicle))
	{
		FurthestReachedDistace = FMath::Max(FurthestReachedDistace, GearVehicle->DistanaceAlongTrack);
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

	case EGearMatchState::Scoreboard:
		Racing_End();
		Scoreboard_Start();
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
	if (!IsValid(VehicleCamera))
	{
		VehicleCamera = GetWorld()->SpawnActor<AVehicleCamera>(VehicleCameraClass);
	}

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AGearPlayerController* PlayerController = Cast<AGearPlayerController>(*It);
		if (IsValid(PlayerController) && PlayerController->IsLocalController())
		{
			PlayerController->ClientStateRacing_Start(LastGameStateTransitionTime);
			PlayerController->SetViewTarget(VehicleCamera);
		}
	}
}

void AGearGameState::Racing_End()
{
	if (IsValid(VehicleCamera))
	{
		VehicleCamera->Destroy();
	}

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AGearPlayerController* PlayerController = Cast<AGearPlayerController>(*It);
		if (IsValid(PlayerController) && PlayerController->IsLocalController())
		{
			PlayerController->ClientStateRacing_End();
		}
	}
}

void AGearGameState::Scoreboard_Start()
{
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AGearPlayerController* PlayerController = Cast<AGearPlayerController>(*It);
		if (IsValid(PlayerController) && PlayerController->IsLocalController())
		{
			PlayerController->ClientStateScoreboard_Start(LastGameStateTransitionTime, CheckpointResults);
		}
	}

	for (APlayerState* PlayerState : PlayerArray)
	{
		AGearPlayerState* GearPlayerState = Cast<AGearPlayerState>(PlayerState);
		check(GearPlayerState);

		GearPlayerState->UpdateRoundScore(CheckpointResults);
	}
}

void AGearGameState::Scoreboard_End()
{
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AGearPlayerController* PlayerController = Cast<AGearPlayerController>(*It);
		if (IsValid(PlayerController) && PlayerController->IsLocalController())
		{
			PlayerController->ClientStateScoreboard_End();
		}
	}
}

float AGearGameState::GetEstimatedScoreboardLifespan() const
{
	return 1.0f;
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

void AGearGameState::ClearCheckpointResults()
{
	const int AvaliableCheckpointNum = CheckpointsStack.Num() - 1;
	CheckpointResults.Empty(AvaliableCheckpointNum);

	for (int i = 0; i < AvaliableCheckpointNum; i++)
	{
		CheckpointResults.AddDefaulted();
	}
}

void AGearGameState::RegisterVehicleAtCheckpoint(AGearVehicle* Vehicle, int CheckpointIndex)
{
	Vehicle->TargetCheckpoint = CheckpointIndex + 1;
	Vehicle->UpdateDistanceAlongTrack();
	UpdateFurthestDistanceWithVehicle(Vehicle);
	//Vehicle->AddTickPrerequisiteActor()

	Vehicles.Add(Vehicle);
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