// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GearTypes.h"
#include "LobbyGameState.generated.h"

class ALobbyPlayerState;
class ALobbyPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNumAllowedPlayerChanged, int32, NumAllowedPlayers);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWinningRequiredScoreChanged, int32, WinningRequiredScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPasswordChanged, FString, Password);

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

	UPROPERTY(ReplicatedUsing=OnRep_WinningRequiredScore, BlueprintReadOnly, EditAnywhere)
	int32 WinningRequiredScore;

	UPROPERTY(ReplicatedUsing=OnRep_Password, BlueprintReadOnly, EditAnywhere)
	FString Password;

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
	
	UFUNCTION(BlueprintPure)
	int32 GetNumAllowedPlayers();

	UPROPERTY(BlueprintAssignable)
	FOnWinningRequiredScoreChanged OnWinningRequiredScoreChanged;

	UFUNCTION(BlueprintCallable)
	void SetWinningRequiredScore(int32 InWinningRequiredScore);
	
	UFUNCTION(BlueprintPure)
	int32 GetWinningRequiredScore();

	UPROPERTY(BlueprintAssignable)
	FOnPasswordChanged OnPasswordChanged;

	UFUNCTION(BlueprintCallable)
	void SetPassword(const FString& InPassword);

	FString ValidatePassword(const FString& InPassword);
	
	UFUNCTION(BlueprintPure)
	bool HasPassword();

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
	
	UFUNCTION()
	void OnRep_WinningRequiredScore();

	UFUNCTION()
	void OnRep_Password();
};