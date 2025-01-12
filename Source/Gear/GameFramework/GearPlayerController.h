// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GearTypes.h"
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

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* MoveScreenAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* PinchAction;

	FVector2D LastTouch_1;
	FVector2D LastTouch_2;
	bool bDraggingTouch_1;
	bool bDraggingTouch_2;

	void UpdateScreenDragValueAndInjectInput();
	void UpdatePinchValueAndInjectInput();

public:

	void Tick( float DeltaSeconds ) override;

	bool IsReady;

	UFUNCTION(Client, Unreliable)
	void PeekClientIsReady();

	UFUNCTION(Server, Unreliable)
	void RespondClientIsReady();

	void OnNewPlayer(AGearPlayerState* GearPlayer);
	void OnRemovePlayer(AGearPlayerState* GearPlayer);


	UFUNCTION(Server, Reliable)
	void SelectPlaceable(AGearPlaceable* Placeable);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void PlaceRoadModule(TSubclassOf<AGearRoadModule> RoadModule, bool bMirrorX);

	UFUNCTION(Server, Reliable)
	void PlaceHazard(class UHazardSocketComponent* TargetSocket);

	void ClickedOnHazardSocketMarker(class AHazardSocketMarker* HazardSocketMarker);

	void ClientStateAllPlayersJoined();
	void ClientStateAllPlayersJoined_End();
	void ClientStateMatchStarted();
	void ClientStateSelectingPieces(float StateStartTime);

	void ClientStatePlacing(float StateStartTime);
	void ClientStatePlacing_Finish();
	
	void ClientStateRacing_Start(float StateStartTime);
	void ClientStateRacing_End();

	void ClientStateScoreboard_Start(float StateStartTime, const TArray<FCheckpointResult>& RoundResults);
	void ClientStateScoreboard_End();

	void ClientStateGameFinished(float StateStartTime);

	void NotifyFurthestReachedCheckpoint(int32 FurthestReachedCheckpoint, int32 CheckpointsNum, float ReachTime);
};