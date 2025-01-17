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
	GearMatchState = EGearMatchState::WaitingForPlayerToJoin;

	LastPlacedCheckpointModuleStackIndex = 0;
	FurthestReachedDistace = 0.0f;
	FurthestReachedCheckpoint = 0;
	RoundNumber = 0;

	WorldMin = FVector::ZeroVector;
	WorldMax = FVector::ZeroVector;

	ServerTimeDelay = 0.0f;
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

void AGearGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AGearGameState, GearMatchState);
	DOREPLIFETIME(AGearGameState, LastGameStateTransitionTime);
	DOREPLIFETIME(AGearGameState, CheckpointsStack);
	DOREPLIFETIME(AGearGameState, Vehicles);
	DOREPLIFETIME(AGearGameState, CheckpointResults);
	DOREPLIFETIME(AGearGameState, RoundNumber);
	DOREPLIFETIME(AGearGameState, LastCountDownTime);
	DOREPLIFETIME(AGearGameState, FurthestReachedCheckpoint);
	DOREPLIFETIME(AGearGameState, FurthestReachedCheckpointTime);
	DOREPLIFETIME(AGearGameState, RoadModuleSocketTransform);
	DOREPLIFETIME(AGearGameState, WorldMin);
	DOREPLIFETIME(AGearGameState, WorldMax);
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

	check(IsValid(NotifictionBoardClass));

	NotifictionBoardWidget = CreateWidget<UNotifictionBoardWidget>(GetWorld(),  NotifictionBoardClass);
	NotifictionBoardWidget->AddToViewport();
}

void AGearGameState::Destroyed()
{
	Super::Destroyed();

	if (IsValid(TrackSpline))
	{
		TrackSpline->Destroy();
	}

	if (IsValid(NotifictionBoardWidget))
	{
		NotifictionBoardWidget->RemoveFromParent();
		NotifictionBoardWidget = nullptr;
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
		break;

	case EGearMatchState::AllPlayersJoined:
		AllPlayerJoined_End();
		break;

	case EGearMatchState::SelectingPlaceables:
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

		break;

	default:
		break;
	}

	// ----------------------------------------------------

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
		Racing_Start();
		break;

	case EGearMatchState::Scoreboard:
		Scoreboard_Start();
		break;

	case EGearMatchState::GameFinished:
		GameFinished();
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

void AGearGameState::AllPlayerJoined_End()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AGearPlayerController* GearController = Cast<AGearPlayerController>(*Iterator);
		if (IsValid(GearController) && GearController->IsLocalController())
		{
			GearController->ClientStateAllPlayersJoined_End();
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
	MarkActorsEnabled();

	FurthestReachedDistace = 0;

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
	MarkActorsIdle();

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
	if (IsValid(VehicleCamera))
	{
		VehicleCamera->Destroy();
	}

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AGearPlayerController* PlayerController = Cast<AGearPlayerController>(*It);
		if (IsValid(PlayerController) && PlayerController->IsLocalController())
		{
			PlayerController->ClientStateScoreboard_End();
		}
	}
}

void AGearGameState::GameFinished()
{
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AGearPlayerController* PlayerController = Cast<AGearPlayerController>(*It);
		if (IsValid(PlayerController) && PlayerController->IsLocalController())
		{
			PlayerController->ClientStateGameFinished(LastGameStateTransitionTime);
		}
	}
}

float AGearGameState::GetEstimatedScoreboardLifespan() const
{	
	return (CheckpointResults.Num() + 3) * UGameVariablesBFL::GV_ScoreboardTimeStep();
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
	CheckpointResults.Empty(AvaliableCheckpointNum);

	for (int i = 0; i < AvaliableCheckpointNum; i++)
	{
		CheckpointResults.AddDefaulted();
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
	return Time > 0 && Time < UGameVariablesBFL::GV_CountDownDuration();
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

void AGearGameState::BroadcastEliminationEvent_Multi_Implementation(AGearPlayerState* PlayerState, EElimanationReason ElimanationReason)
{
	if (IsValid(NotifictionBoardWidget))
	{
		NotifictionBoardWidget->NotifyElimination(PlayerState, ElimanationReason);
	}
}

void AGearGameState::BroadcastReachedCheckpointEvent_Multi_Implementation(AGearPlayerState* PlayerState, ACheckpoint* Checkpoint, int32 Position)
{
	if (IsValid(NotifictionBoardWidget))
	{
		NotifictionBoardWidget->NotifyReachedCheckpoint(PlayerState, Checkpoint, Position);
	}
}

void AGearGameState::OnRep_FurthestReachedCheckpoint()
{
	if (GearMatchState == EGearMatchState::Racing)
	{
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			AGearPlayerController* PlayerController = Cast<AGearPlayerController>(*It);
			if (IsValid(PlayerController) && PlayerController->IsLocalController())
			{
				PlayerController->NotifyFurthestReachedCheckpoint(FurthestReachedCheckpoint, CheckpointsStack.Num(), FurthestReachedCheckpointTime);
			}
		}
	}
}