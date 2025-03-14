// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearGameState.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearBuilderPawn.h"
#include "GameFramework/GearCameraManager.h"

#include "Placeable/GearRoadModule.h"
#include "Placeable/GearHazard.h"

#include "UI/NotifictionBoardWidget.h"

#include "GameSystems/Checkpoint.h"
#include "GameSystems/TrackSpline.h"

#include "Vehicle/GearVehicle.h"
#include "Vehicle/VehicleCamera.h"

#include "Utils/GameVariablesBFL.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"

AGearGameState::AGearGameState()
{
	GearMatchState = EGearMatchState::None;

	LastPlacedCheckpointModuleStackIndex = 0;
	FurthestReachedDistace = 0.0f;
	FurthestReachedCheckpoint = 0;
	RoundNumber = 0;

	WorldMin = FVector::ZeroVector;
	WorldMax = FVector::ZeroVector;

	ServerTimeDelay = 0.0f;
	bEveryPlayerReady = false;
}

void AGearGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearGameState, GearMatchState);
	DOREPLIFETIME(AGearGameState, bEveryPlayerReady);
	DOREPLIFETIME(AGearGameState, LastGameStateTransitionTime);
	DOREPLIFETIME(AGearGameState, CheckpointsStack);
	DOREPLIFETIME(AGearGameState, Vehicles);
	DOREPLIFETIME(AGearGameState, RoundsResult);
	DOREPLIFETIME(AGearGameState, RoundNumber);
	DOREPLIFETIME(AGearGameState, LastCountDownTime);
	DOREPLIFETIME(AGearGameState, FurthestReachedCheckpoint);
	DOREPLIFETIME(AGearGameState, FurthestReachedCheckpointTime);
	DOREPLIFETIME(AGearGameState, RoadModuleSocketTransform);
	DOREPLIFETIME(AGearGameState, WorldMin);
	DOREPLIFETIME(AGearGameState, WorldMax);
}

void AGearGameState::PostNetInit()
{
	Super::PostNetInit();


}

void AGearGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	for (AGearVehicle* Vehicle : Vehicles)
	{
		if (IsValid(Vehicle) && !Vehicle->IsSpectating())
		{
			UpdateFurthestDistanceWithVehicle(Vehicle);
		}
	}
}

class AGearPlayerController* AGearGameState::GetLocalPlayer()
{
	if (!IsValid(LocalPlayer))
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (Iterator->Get()->IsLocalController())
			{
				LocalPlayer = Cast<AGearPlayerController>(Iterator->Get());
			}
		}
	}

	return LocalPlayer;
}

void AGearGameState::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(TrackSpline))
	{
		TrackSpline = Cast<ATrackSpline>(UGameplayStatics::GetActorOfClass(GetWorld(), ATrackSpline::StaticClass()));
		if (!IsValid(TrackSpline))
		{
			UE_LOG(LogTemp, Warning, TEXT("There is no track spline actor in level!"));
		}
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
	switch (OldState)
	{
	case EGearMatchState::WaitingForPlayerToJoin:
		Waiting_End();
		break;

	case EGearMatchState::SelectingPlaceables:
		Selecting_End();
		break;

	case EGearMatchState::Placing:
		Placing_End();
		break;

	case EGearMatchState::Racing:
		Racing_End();
		break;

	case EGearMatchState::Scoreboard:
		Scoreboard_End();
		break;

	case EGearMatchState::GameFinished:
		Finishboard_End();
		break;

	default:
		break;
	}

	// ----------------------------------------------------

	switch (GearMatchState)
	{
	case EGearMatchState::WaitingForPlayerToJoin:
		Waiting_Start();
		break;

	case EGearMatchState::SelectingPlaceables:
		Selecting_Start();
		break;

	case EGearMatchState::Placing:
		Placing_Start();
		break;

	case EGearMatchState::Racing:
		Racing_Start();
		break;

	case EGearMatchState::Scoreboard:
		Scoreboard_Start();
		break;

	case EGearMatchState::GameFinished:
		Finishboard_Start();
		break;

	default:
		break;
	}
}

void AGearGameState::OnModuleStackChanged()
{
	if (HasAuthority())
	{
		if (IsValid(TrackSpline))
		{
			TrackSpline->RoadModuleStackChanged(RoadModuleStack);
		}

		RoadModuleSocketTransform = RoadModuleStack.Top()->GetAttachableSocket()->GetComponentTransform();
		OnRep_RoadModuleSocketTransform();
	}
}

void AGearGameState::OnRep_RoadModuleSocketTransform()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (Iterator->Get()->IsLocalController())
		{
			AGearBuilderPawn* BuilderPawn = Iterator->Get()->GetPawn<AGearBuilderPawn>();
			if (IsValid(BuilderPawn))
			{
				BuilderPawn->OnRoadModuleSocketChanged();
			}
		}
	}
}

void AGearGameState::Waiting_Start()
{
	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->ClientStateWaiting_Start(LastGameStateTransitionTime);
	}

	for (APlayerState* Player : PlayerArray)
	{
		AGearPlayerState* GearPlayer = Cast<AGearPlayerState>(Player);

		if (IsValid(GearPlayer))
		{
			LocalPlayer->OnPlayerJoined(GearPlayer);
		}
	}
}

void AGearGameState::Waiting_End()
{
	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->ClientStateWaiting_End();
	}
}

void AGearGameState::Selecting_Start()
{
	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->ClientStateSelecting_Start(LastGameStateTransitionTime);
	}
}

void AGearGameState::Selecting_End()
{
	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->ClientStateSelecting_End();
	}
}

void AGearGameState::Placing_Start()
{
	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->ClientStatePlacing_Start(LastGameStateTransitionTime);
	}
}

void AGearGameState::Placing_End()
{
	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->ClientStatePlacing_End();
	}
}

void AGearGameState::Racing_Start()
{
	MarkActorsEnabled();

	FurthestReachedDistace = 0;

	if (!IsValid(VehicleCamera))
	{
		VehicleCamera = GetWorld()->SpawnActor<AVehicleCamera>(VehicleCameraClass);
	}

	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->ClientStateRacing_Start(LastGameStateTransitionTime);
		LocalPlayer->SetViewTarget(VehicleCamera);
	}
}

void AGearGameState::Racing_End()
{
	MarkActorsIdle();

	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->ClientStateRacing_End();
	}
}

void AGearGameState::Scoreboard_Start()
{
	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->ClientStateScoreboard_Start(LastGameStateTransitionTime);
	}

// 	for (APlayerState* PlayerState : PlayerArray)
// 	{
// 		AGearPlayerState* GearPlayerState = Cast<AGearPlayerState>(PlayerState);
// 		check(GearPlayerState);
// 
// 		GearPlayerState->UpdateRoundScore(CheckpointResults);
// 	}
}

void AGearGameState::Scoreboard_End()
{
	if (IsValid(VehicleCamera))
	{
		VehicleCamera->Destroy();
	}

	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->ClientStateScoreboard_End();
	}
}

void AGearGameState::Finishboard_Start()
{
	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->ClientStateFinishboard_Start(LastGameStateTransitionTime);
	}
}

void AGearGameState::Finishboard_End()
{
	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->ClientStateFinishboard_End();
	}
}

void AGearGameState::NotifyAllPlayersJoined()
{
	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->NotifyAllPlayerJoined();
	}
}

float AGearGameState::GetEstimatedScoreboardLifespan() const
{	
	//return (CheckpointResults.Num() + 3) * UGameVariablesBFL::GV_ScoreboardTimeStep();

	int32 MaxCheckpointSteps = 0;

	for (TObjectPtr<APlayerState> PlayerState : PlayerArray)
	{
		AGearPlayerState* Player = Cast<AGearPlayerState>(PlayerState.Get());
	
		if (IsValid(Player))
		{
			int NumPassedCheckpoint = 0;
			int NumGoldCheckpoint = 0;

			for (const FCheckpointResult& Result : RoundsResult.Last().CheckpointArray)
			{
				int PlayerIndex = Result.PlayerList.Find(Player);

				if (PlayerIndex > INDEX_NONE)
				{
					NumPassedCheckpoint++;
				}

				if (PlayerIndex == 0)
				{
					NumGoldCheckpoint++;
				}
			}

			MaxCheckpointSteps = FMath::Max(MaxCheckpointSteps, NumPassedCheckpoint * UGameVariablesBFL::GV_FinishingCheckpointScore() 
				+ NumGoldCheckpoint * UGameVariablesBFL::GV_FirstFinishAdditionalScore());
		}
	}

	float Result = UGameVariablesBFL::GV_ScoreboardTimeDelay() * 2 + UGameVariablesBFL::GV_ScoreboardTimeStep() * MaxCheckpointSteps;
	UE_LOG(LogTemp, Warning, TEXT("esstimated %f seconds for scoreboard"), Result);
	
	return Result;
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

	OnModuleStackChanged();
	
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

USceneComponent* AGearGameState::GetRoadStackAttachableSocket()
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

ACheckpoint* AGearGameState::GetFurthestReachedCheckpoint() const
{
	return FurthestReachedCheckpoint < CheckpointsStack.Num() - 1 ? CheckpointsStack[FurthestReachedCheckpoint] : nullptr;
}

ACheckpoint* AGearGameState::GetNextFurthestReachedCheckpoint() const
{
	return FurthestReachedCheckpoint < CheckpointsStack.Num() - 2 ? CheckpointsStack[FurthestReachedCheckpoint + 1] : nullptr;
}

void AGearGameState::UpdateRoadModuleSocket()
{
	if (HasAuthority())
	{
		RoadModuleSocketTransform = RoadModuleStack.Top()->GetAttachableSocket()->GetComponentTransform();
	}
}

void AGearGameState::MarkActorsIdle()
{
	if (HasAuthority())
	{
		for (AGearRoadModule* RoadModule : RoadModuleStack)
		{
			RoadModule->SetIdle();
		}

		TArray<AActor*> Hazards;
		UGameplayStatics::GetAllActorsOfClass(this, AGearHazard::StaticClass(), Hazards);

		for (AActor* HazardActor : Hazards)
		{
			AGearPlaceable* Hazard = Cast<AGearPlaceable>(HazardActor);
			if (IsValid(Hazard))
			{
				Hazard->SetIdle();
			}
		}
	}
}

void AGearGameState::MarkActorsEnabled()
{
	if (HasAuthority())
	{
		for (AGearRoadModule* RoadModule : RoadModuleStack)
		{
			RoadModule->SetEnabled();
		}

		TArray<AActor*> Hazards;
		UGameplayStatics::GetAllActorsOfClass(this, AGearHazard::StaticClass(), Hazards);

		for (AActor* HazardActor : Hazards)
		{
			AGearPlaceable* Hazard = Cast<AGearPlaceable>(HazardActor);
			if (IsValid(Hazard))
			{
				Hazard->SetEnabled();
			}
		}
	}
}

void AGearGameState::ClearOccupiedVehicleStarts()
{
	UE_LOG(LogTemp, Warning, TEXT("cleared occupied vehicle starts"));

	for (ACheckpoint* Checkpoint : CheckpointsStack)
	{
		Checkpoint->ClearOccupied();
	}
}

void AGearGameState::ClearCheckpointResults()
{
	const int AvaliableCheckpointNum = CheckpointsStack.Num() - 1;
	OngoingRoundResult.CheckpointArray.Empty(AvaliableCheckpointNum);

	for (int i = 0; i < AvaliableCheckpointNum; i++)
	{
		OngoingRoundResult.CheckpointArray.AddDefaulted();
	}
}

void AGearGameState::RegisterVehicleAtCheckpoint(AGearVehicle* Vehicle, ACheckpoint* Checkpoint)
{
	Vehicle->TargetCheckpoint = Checkpoint->CheckpointIndex + 1;
	Vehicle->UpdateDistanceAlongTrack();

	if (!Vehicle->IsSpectating())
	{
		UpdateFurthestDistanceWithVehicle(Vehicle);
	}

	Vehicles.Add(Vehicle);
}

TArray<AGearPlayerState*> AGearGameState::GetWinningPlayers() const
{
	TArray<AGearPlayerState*> WinningPlayers;

	for (APlayerState* PlayerState : PlayerArray)
	{
		AGearPlayerState* GearPlayerState = Cast<AGearPlayerState>(PlayerState);
		check(GearPlayerState);

		if (GearPlayerState->IsWinner())
		{
			WinningPlayers.Add(GearPlayerState);
		}
	}

	return WinningPlayers;
}

bool AGearGameState::IsAnyPlayerWinning() const
{
	return GetWinningPlayers().Num() > 0;
}

TArray<AGearPlayerState*> AGearGameState::GetPlayersPlacement()
{
	TArray<AGearPlayerState*> Results;

	for (APlayerState* PlayerState : PlayerArray)
	{
		AGearPlayerState* GearPlayerState = Cast<AGearPlayerState>(PlayerState);
		check(GearPlayerState);

		Results.Add(GearPlayerState);
	}

	Results.Sort(
	[](const AGearPlayerState& p1, const AGearPlayerState& p2){
		return p1.CurrentScore > p2.CurrentScore;
	});

	return Results;
}

void AGearGameState::UpdateFurthestReachedCheckpoint(int32 CheckpointIndex)
{
	if (HasAuthority())
	{
		FurthestReachedCheckpoint = CheckpointIndex;
		FurthestReachedCheckpointTime = GetWorld()->GetTimeSeconds();
		OnRep_FurthestReachedCheckpoint();
	}
}

bool AGearGameState::IsCountDown()
{
	float Time = GetServerWorldTimeSeconds() - LastCountDownTime;
	return Time > 0 && Time < UGameVariablesBFL::GV_CountDownDuration() + UGameVariablesBFL::GV_InformationPanelDuration();
}

float AGearGameState::TimeFromLastTransition() const
{
	return GetServerWorldTimeSeconds() - LastGameStateTransitionTime;
}

void AGearGameState::GetWorldBounds(FVector& World_Min, FVector& World_Max)
{
	World_Min = WorldMin;
	World_Max = WorldMax;
}

int32 AGearGameState::GetSpectatingPlayerCount()
{
	int32 Count = 0;
	for (AGearVehicle* Vehicle : Vehicles)
	{
		Count = Vehicle->IsSpectating() ? Count + 1 : Count;
	}

	return Count;
}

int32 AGearGameState::GetDrivingPlayerCount()
{
	int32 Count = 0;
	for (AGearVehicle* Vehicle : Vehicles)
	{
		Count = Vehicle->IsSpectating() ? Count : Count + 1;
	}

	return Count;
}

void AGearGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (IsValid(GetLocalPlayer()))
	{
		AGearPlayerState* GearPlayer = Cast<AGearPlayerState>(PlayerState);
		if (IsValid(GearPlayer))
		{
			LocalPlayer->OnPlayerJoined(GearPlayer);
		}
	}
}

void AGearGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	if (IsValid(GetLocalPlayer()))
	{
		AGearPlayerState* GearPlayer = Cast<AGearPlayerState>(PlayerState);
		if (IsValid(GearPlayer))
		{
			LocalPlayer->OnPlayerQuit(GearPlayer);
		}
	}
}

void AGearGameState::OnRep_EveryPlayerReady()
{
	if (bEveryPlayerReady)
	{
		NotifyAllPlayersJoined();
	}
}

void AGearGameState::SetServerTimeDelay(float InServerTimeDelay)
{
	ServerTimeDelay = InServerTimeDelay;
}

float AGearGameState::GetServerWorldTime()
{
	return GetWorld()->GetTimeSeconds() + ServerTimeDelay;
}

void AGearGameState::BroadcastSelectedEvent_Multi_Implementation(AGearPlayerState* PlayerState, TSubclassOf<AGearPlaceable> PlaceableClass, AGearPlaceable* Placeable)
{
	check(SelectedSound);
	check(SelectedFX);

	UGameplayStatics::PlaySound2D(GetWorld(), SelectedSound);

	if (IsValid(Placeable))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SelectedFX, Placeable->PreviewRotationPivot->GetComponentLocation());
	}
}

void AGearGameState::BroadcastPlacedEvent_Multi_Implementation(AGearPlayerState* PlayerState, TSubclassOf<AGearPlaceable> PlaceableClass)
{
	check(PlacedSound);
	UGameplayStatics::PlaySound2D(GetWorld(), PlacedSound);
	
// 	if (IsValid(NotifictionBoardWidget))
// 	{
// 		NotifictionBoardWidget->NotifyPlayerPlaced(PlayerState, PlaceableClass);
// 	}
}

void AGearGameState::BroadcastEliminationEvent_Multi_Implementation(AGearPlayerState* PlayerState, EElimanationReason ElimanationReason, float EliminationTime, int32 RemainingPlayersCount)
{
	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->OnPlayerEliminated(PlayerState, ElimanationReason, EliminationTime, RemainingPlayersCount);
	}
}

void AGearGameState::BroadcastReachedCheckpointEvent_Multi_Implementation(AGearPlayerState* PlayerState, ACheckpoint* Checkpoint, int32 Position, int32 AllCheckpointNum, float ReachTime)
{
	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->OnReachedCheckpoint(PlayerState, Checkpoint, Position, AllCheckpointNum, ReachTime);
	}
}

void AGearGameState::BroadcastRaceStartEvent_Multi_Implementation(float StartTime, bool bWithCountDown)
{
	if (IsValid(GetLocalPlayer()))
	{
		LocalPlayer->OnRaceStart(StartTime, bWithCountDown);
	}
}

void AGearGameState::OnRep_FurthestReachedCheckpoint()
{
	
}