// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GearTypes.h"
#include "GameSystems/InputHnadler.h"
#include "LobbyPlayerController.generated.h"

class ALobbyPlayerState;

/**
 * 
 */
UCLASS()
class GEAR_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* MoveScreenAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* PinchAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UInputAction* TouchEndAction;

	virtual void PostNetInit() override;
	void BeginPlay() override;

	void OnRep_PlayerState() override;

	void AddLobbyMenu();
	
	UFUNCTION(Server, Reliable)
	void Server_SetPlayerName(const FString& PlayerName);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAddPlayer(ALobbyPlayerState* InPlayer);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRemovePlayer(ALobbyPlayerState* InPlayer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UUserWidget> LobbyWidgetClass;

#if PLATFORM_WINDOWS

	FMouseInputHandler RightMouseInputHandler;
	FMouseInputHandler LeftMouseInputHandler;

#elif PLATFORM_ANDROID

	FTouchInputHandler Touch1_InputHandler;
	FTouchInputHandler Touch2_InputHandler;

#endif

public:

	virtual void Tick( float DeltaSeconds ) override;

	void NotifyNewPlayer(APlayerState* InPlayer);
	void NotifyRemovePlayer(APlayerState* InPlayer);

	UFUNCTION(Server, Reliable)
	void RequestColorChange(EPlayerColorCode Color);

	UFUNCTION(Client, Reliable)
	void ColorChange_RollBack(EPlayerColorCode Color);

	UFUNCTION(BlueprintCallable)
	void TryChangeColor(EPlayerColorCode Color);

	ALobbyPlayerState* GetLobbyPlayerState();
};
