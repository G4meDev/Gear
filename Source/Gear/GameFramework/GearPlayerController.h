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

	FVector2D LastDragPosition;
	bool bDraging;

	void UpdateScreenDragValueAndInjectInput();

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
	void PlaceRoadModule(TSubclassOf<AGearRoadModule> RoadModule, UPlaceableSocket* TargetSocket, bool bMirrorX);

	void ClientStateAllPlayersJoined();
	void ClientStateMatchStarted();
	void ClientStateSelectingPieces(float StateStartTime);

	void ClientStatePlacing(float StateStartTime);
	void ClientStatePlacing_Finish();
	
	void ClientStateRacing_Start(float StateStartTime);
	void ClientStateRacing_End();

	void ClientStateScoreboard_Start(float StateStartTime, const TArray<FCheckpointResult>& RoundResults);
	void ClientStateScoreboard_End();

};