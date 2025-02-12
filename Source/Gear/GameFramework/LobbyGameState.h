// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GearTypes.h"
#include "LobbyGameState.generated.h"

class ALobbyPlayerState;
class ALobbyPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNumAllowedPlayerChanged, int32, NumAllowedPlayers);

/**
 * 
 */
UCLASS()
class GEAR_API ALobbyGameState : public AGameState
{
	GENERATED_BODY()
	

protected:

	UPROPERTY()
	TArray<class ALobbyPlayerPlatform*> PlayerPlatforms;
	
	UPROPERTY(ReplicatedUsing=OnRep_NumAllowedPlayers, BlueprintReadOnly, EditAnywhere)
	int32 NumAllowedPlayers;

	friend class ALobbyGameMode;

public:

	ALobbyGameState();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void RequestColorChangeForPlayer(ALobbyPlayerController* PC, EPlayerColorCode Color);

	UFUNCTION(BlueprintPure)
	TArray<ALobbyPlayerState*> GetLobbyPlayers();

	UFUNCTION(BlueprintPure)
	TArray<ALobbyPlayerState*> GetJoinTimeSortedLobbyPlayers();

	void ReconstructPlayersPlatform();

	UFUNCTION(BlueprintPure)
	bool IsWaitingForPlayers();

	UPROPERTY(BlueprintAssignable)
	FOnNumAllowedPlayerChanged OnNumberOfAllowedPlayerChanged;

	UFUNCTION(BlueprintCallable)
	void SetNumAllowedPlayers(int32 InAllowedNumberOfPlayers);

protected:

	void AddPlayerState(APlayerState* PlayerState) override;
	void RemovePlayerState(APlayerState* PlayerState) override;

	void GetInUseColors(TArray<EPlayerColorCode>& InUseColors);
	void AssignNewColorToPlayer(APlayerState* Player);

	bool CanStartGame();

	void StartGame();
	FTimerHandle StartGameTimerHandle;

	UFUNCTION(NetMulticast, Reliable)
	void StartGame_Multi(float StartTime);

	UFUNCTION(NetMulticast, Reliable)
	void StartGameCancel_Multi();

	ELobbyGameState LobbyGameState;

	UFUNCTION()
	void OnRep_NumAllowedPlayers();
};