// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GearGameInstance.generated.h" 

class UGearSaveGame;

/**
 * 
 */
UCLASS()
class GEAR_API UGearGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	void Init() override;

	UFUNCTION(BlueprintPure)
	FString GetPlayerName();

	UFUNCTION(BlueprintCallable)
	FString TryChangePlayerName(const FString& NewName);

private:
	UPROPERTY()
	UGearSaveGame* GearSave;

	void InitPersistantData();

	void OnAsyncSaveFinished(const FString& SlotName, int32 UserIndex, bool bSucced);

	bool TryLoadGame();
	bool TrySaveGame();
	void TrySaveGameAsync();

	FString ValidtePlayerNameChange(const FString& NewName, const FString& OldName);
};
