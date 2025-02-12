// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameFramework/GearTypes.h"
#include "GearGameInstance.generated.h" 

class UGearSaveGame;
class UUserWidget;
class SWidget;

/**
 * 
 */
UCLASS()
class GEAR_API UGearGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	void Init() override;

	virtual void Shutdown() override;

	UFUNCTION(BlueprintPure)
	FString GetPlayerName();

	UFUNCTION(BlueprintCallable)
	FString TryChangePlayerName(const FString& NewName);

	void OnSeamlessTravelStart(UWorld* World, const FString& Str);

	void OnSeamlessTravelTransition(UWorld* World);

	UFUNCTION(BlueprintCallable)
	void ShowLoadingScreen();

	UFUNCTION(BlueprintCallable)
	void RemoveLoadingScreen();


	UFUNCTION(BlueprintCallable)
	void SetSoundAmplitude(float Amplitude);

	UFUNCTION(BlueprintCallable)
	void SetMusicAmplitude(float Amplitude);

	UFUNCTION(BlueprintCallable)
	void SetQualityLevel(int32 Level);

	UFUNCTION(BlueprintCallable)
	void SetLanguage(ELanguageOption Language);

	UFUNCTION(BlueprintPure)
	float GetSoundAmplitude();

	UFUNCTION(BlueprintPure)
	float GetMusicAmplitude();

	UFUNCTION(BlueprintPure)
	int32 GetQualityLevel();

	UFUNCTION(BlueprintPure)
	ELanguageOption GetLanguage ();

	void SetDisconnectionReason(EPlayerDisconnectionReason InDisconnectionReason);

	UFUNCTION(BlueprintPure)
	EPlayerDisconnectionReason GetDisconnectionReason();

protected:
	virtual void ReturnToMainMenu() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LoadingScreenWidgetClass;

	TSharedPtr<SWidget> LoadingScreenWidget;
	
	
	UPROPERTY(EditDefaultsOnly)
	USoundClass* BaseSoundClass;

	UPROPERTY(EditDefaultsOnly)
	USoundClass* MusicSoundClass;

	UPROPERTY(EditDefaultsOnly)
	class USoundMix* BaseSoundMix;

	EPlayerDisconnectionReason DisconnectionReason;

	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	

private:
	UPROPERTY()
	UGearSaveGame* GearSave;

	void InitPersistantData();

	void OnAsyncSaveFinished(const FString& SlotName, int32 UserIndex, bool bSucced);

	bool TryLoadGame();
	bool TrySaveGame();
	void TrySaveGameAsync();

	FTimerHandle DirtySettingSaveTimerHandle;
	void MarkUserSettingsDirty();

	void ApplyUserSettings();

	FString ValidtePlayerNameChange(const FString& NewName, const FString& OldName);

	void ChangePerformanceSettings(bool bEnabingLoadingScreen);
};