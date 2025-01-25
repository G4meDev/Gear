// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GearTypes.h"
#include "GameSystems/InputHnadler.h"
#include "GearPlayerController.generated.h"

class AGearPlayerState;
class AGearPlaceable;
class AGearRoadModule;
class UPlaceableSocket;
class AGearHUD;
class UInputMappingContext;
class UInputAction;

/**
 * 
 */
UCLASS()
class GEAR_API AGearPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	AGearPlayerController();

	void PostInitializeComponents() override;

	virtual void PostNetInit() override;

	virtual void BeginPlay() override;

	class AGearGameState* GearGameState;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* MoveScreenAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* PinchAction;

	void UpdateAndInjectInputs();

#if PLATFORM_WINDOWS

	FMouseInputHandler RightMouseInputHandler;
	FMouseInputHandler LeftMouseInputHandler;

#elif PLATFORM_ANDROID

	FTouchInputHandler Touch1_InputHandler;
	FTouchInputHandler Touch2_InputHandler;

#endif

public:

	void Tick( float DeltaSeconds ) override;

	bool IsReady;

	UFUNCTION(Client, Unreliable)
	void PeekClientIsReady();

	UFUNCTION(Server, Unreliable)
	void RespondClientIsReady();

	void OnPlayerJoined(AGearPlayerState* GearPlayer);
	void OnPlayerQuit(AGearPlayerState* GearPlayer);

	void OnPlayerEliminated(AGearPlayerState* EliminatedPlayer, EElimanationReason ElimanationReason, float EliminationTime, int32 RemainingPlayersCount);
	void OnReachedCheckpoint(AGearPlayerState* ReachedPlayer, class ACheckpoint* Checkpoint, int32 Position, int32 AllCheckpointNum, float ReachTime);

	void OnRaceStart(float StartTime, bool bWithCountDown);

	UFUNCTION(Server, Reliable)
	void SelectPlaceable(AGearPlaceable* Placeable);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void PlaceRoadModule(TSubclassOf<AGearRoadModule> RoadModule, bool bMirrorX);

	UFUNCTION(Server, Reliable)
	void PlaceHazard(class UHazardSocketComponent* TargetSocket);

	void ClickedOnHazardSocketMarker(class AHazardSocketMarker* HazardSocketMarker);
	
	void ClientStateWaiting_Start(float StartTime);
	void ClientStateWaiting_End();

	void ClientStateSelecting_Start(float StartTime);
	void ClientStateSelecting_End();

	void ClientStatePlacing_Start(float StartTime);
	void ClientStatePlacing_End();
	
	void ClientStateRacing_Start(float StartTime);
	void ClientStateRacing_End();

	void ClientStateScoreboard_Start(float StartTime, const TArray<FCheckpointResult>& RoundResults);
	void ClientStateScoreboard_End();

	void ClientStateFinishboard_Start(float StartTime);
	void ClientStateFinishboard_End();

	void NotifyFurthestReachedCheckpoint(int32 FurthestReachedCheckpoint, int32 CheckpointsNum, float ReachTime);

	void NotifyAllPlayerJoined();

// -------------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly)
	float NetworkClockUpdateFrequency = 1.0f;

	TArray<float> RTTCircularBuffer;

	void RequestWorldTime_Internal();

	UFUNCTION(Server, Unreliable)
	void ServerRequestWorldTime(float ClientTimestamp);

	UFUNCTION(Client, Unreliable)
	void ClientUpdateWorldTime(float ClientTimestamp, float ServerTimestamp);
};