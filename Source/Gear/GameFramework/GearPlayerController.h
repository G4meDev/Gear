// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GearPlayerController.generated.h"

class AGearPlayerState;
class AGearHUD;
class AGearHazardActor;
class UInputMappingContext;

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

public:

	bool IsReady;

	UFUNCTION(Client, Unreliable)
	void PeekClientIsReady();

	UFUNCTION(Server, Unreliable)
	void RespondClientIsReady();

	void OnNewPlayer(AGearPlayerState* GearPlayer);
	void OnRemovePlayer(AGearPlayerState* GearPlayer);

	UFUNCTION(Client, Reliable)
	void ClientStateAllPlayersJoined();

	UFUNCTION(Client, Reliable)
	void ClientStateMatchStarted();

	UFUNCTION(Server, Reliable)
	void SelectHazard(AGearHazardActor* Hazard);


	UFUNCTION(Client, Reliable)
	void ClientStateSelectingPieces(float StateStartTime);

	UFUNCTION(Client, Reliable)
	void ClientStatePlacingPieces(float StateStartTime);
};