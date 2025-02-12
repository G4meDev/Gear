// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ALobbyGameMode();

	UFUNCTION(BlueprintCallable)
	void KickPlayer(class ALobbyPlayerController* Player);

protected:
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	UFUNCTION(BlueprintPure)
	bool HasEmptySlot();

	UFUNCTION(BlueprintPure)
	bool IsGameStarting();
};