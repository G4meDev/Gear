// Fill out your copyright notice in the Description page of Project Settings

#include "GameFramework/GearGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetInternationalizationLibrary.h"
#include "GameFramework/GearSaveGame.h"
#include "GameFramework/GearTypes.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/SWidget.h"
#include "ShaderPipelineCache.h"
#include "HAL/ThreadHeartBeat.h"
#include "Utils/DataHelperBFL.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameUserSettings.h"
#include "Sound/SoundClass.h"

#define SAVE_SLOT_NAME "Save"
#define SAVE_USER_INDEX 0
#define DIRTY_SAVE_DELAY 1.0f

void UGearGameInstance::Init()
{
	Super::Init();

	DisconnectionReason = EPlayerDisconnectionReason::Quit;

#if WITH_EDITORONLY_DATA

	auto* SplineCurvesStruct = FSplineCurves::StaticStruct();
	auto* BrokenProperty = SplineCurvesStruct->FindPropertyByName(TEXT("Metadata"));
	BrokenProperty->SetPropertyFlags(BrokenProperty->PropertyFlags | EPropertyFlags::CPF_RepSkip);

#endif

	FWorldDelegates::OnSeamlessTravelStart.AddUObject(this, &UGearGameInstance::OnSeamlessTravelStart);
	FWorldDelegates::OnSeamlessTravelTransition.AddUObject(this, &UGearGameInstance::OnSeamlessTravelTransition);

	InitPersistantData();
	ApplyUserSettings();
}

void UGearGameInstance::Shutdown()
{
	//RemoveLoadingScreen();

	LoadingScreenWidget.Reset();

	Super::Shutdown();
}

void UGearGameInstance::ReturnToMainMenu()
{
	Super::ReturnToMainMenu();

}

void UGearGameInstance::InitPersistantData()
{
	if (!TryLoadGame())
	{
		UE_LOG(LogTemp, Warning, TEXT("loading save file failed. trying to make new save file"));

		GearSave = Cast<UGearSaveGame>(UGameplayStatics::CreateSaveGameObject(UGearSaveGame::StaticClass()));
		TrySaveGame();
	}
}

bool UGearGameInstance::TryLoadGame()
{
	USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(SAVE_SLOT_NAME, SAVE_USER_INDEX);
	if (!SaveGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("could not load save file"));
		return false;
	}
	GearSave = Cast<UGearSaveGame>(SaveGame);
	if (!IsValid(GearSave))
	{
		UE_LOG(LogTemp, Warning, TEXT("save file is not compatiable"));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("loading save file successful"));
	return true;
}

bool UGearGameInstance::TrySaveGame()
{
	if(UGameplayStatics::SaveGameToSlot(GearSave, SAVE_SLOT_NAME, SAVE_USER_INDEX))
	{
		UE_LOG(LogTemp, Warning, TEXT("save succesfull"));
		return true;
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("failed to save game"));
		return false;
	}
}

void UGearGameInstance::TrySaveGameAsync()
{
	UGameplayStatics::AsyncSaveGameToSlot(GearSave, SAVE_SLOT_NAME, SAVE_USER_INDEX, FAsyncSaveGameToSlotDelegate::CreateUObject(this, &UGearGameInstance::OnAsyncSaveFinished));
}

void UGearGameInstance::OnAsyncSaveFinished(const FString& SlotName, int32 UserIndex, bool bSucced)
{
	if (bSucced)
	{
		UE_LOG(LogTemp, Warning, TEXT("async save succesfull"));
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("failed to async save game"));
	}
}

void UGearGameInstance::MarkUserSettingsDirty()
{
	GetTimerManager().ClearTimer(DirtySettingSaveTimerHandle);
	GetTimerManager().SetTimer(DirtySettingSaveTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::TrySaveGameAsync), DIRTY_SAVE_DELAY, false);
}

void UGearGameInstance::ApplyUserSettings()
{
	if (IsValid(GearSave))
	{
		FString ScalabilityStr = "Scalability " + FString::FromInt(GearSave->QualityLevel - 1);
		UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), ScalabilityStr);
	
		UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), TEXT("r.Mobile.FSR.enabled 1"));
		UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), TEXT("r.Mobile.FSR.RCAS.enabled 1"));
		UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), TEXT("r.Mobile.FSR.Upsampling.enabled 1"));
		

		UGameplayStatics::SetBaseSoundMix(GetWorld(), BaseSoundMix);
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), BaseSoundMix, BaseSoundClass, GearSave->SoundAmplitude);
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), BaseSoundMix, MusicSoundClass, GearSave->MusicAmplitude);

		UKismetInternationalizationLibrary::SetCurrentCulture(UDataHelperBFL::LanguageOptionToCulture(GearSave->Language));
	}
}

FString UGearGameInstance::ValidtePlayerNameChange(const FString& NewName, const FString& OldName)
{
	//TODO: name validation

	return NewName;
}

FString UGearGameInstance::GetPlayerName() 
{
	if (IsValid(GearSave))
	{
		return GearSave->PlayerName;
	}

	return DEFAULT_PLAYER_NAME;
}

FString UGearGameInstance::TryChangePlayerName(const FString& NewName)
{
	if (IsValid(GearSave))
	{
		FString ValidatedName = ValidtePlayerNameChange(NewName, GetPlayerName());
		GearSave->PlayerName = ValidatedName;
		TrySaveGameAsync();
	}

	return GetPlayerName();
}

void UGearGameInstance::OnSeamlessTravelStart(UWorld* World, const FString& Str)
{
	ShowLoadingScreen();
}

void UGearGameInstance::OnSeamlessTravelTransition(UWorld* World)
{
	RemoveLoadingScreen();
}

// -----------------------------------------------------------------------------------------------

void UGearGameInstance::ShowLoadingScreen()
{
	if (LoadingScreenWidget.IsValid())
	{
		return;
	}

	if (UUserWidget* UserWidget = UUserWidget::CreateWidgetInstance(*this, LoadingScreenWidgetClass, NAME_None))
	{
		LoadingScreenWidget = UserWidget->TakeWidget();
	}

	UGameViewportClient* GameViewportClient = GetGameViewportClient();
	GameViewportClient->AddViewportWidgetContent(LoadingScreenWidget.ToSharedRef(), 0);

	ChangePerformanceSettings(true);

	FSlateApplication::Get().Tick();
}

void UGearGameInstance::RemoveLoadingScreen()
{
	if (LoadingScreenWidget.IsValid())
	{
		if (UGameViewportClient* GameViewportClient = GetGameViewportClient())
		{
			GameViewportClient->RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());
		}

		LoadingScreenWidget.Reset();
	}

	ChangePerformanceSettings(false);
}

void UGearGameInstance::SetSoundAmplitude(float Amplitude)
{
	if (IsValid(GearSave))
	{
		GearSave->SoundAmplitude = FMath::Clamp(Amplitude, 0, 1);
		ApplyUserSettings();
		MarkUserSettingsDirty();
	}
}

void UGearGameInstance::SetMusicAmplitude(float Amplitude)
{
	if (IsValid(GearSave))
	{
		GearSave->MusicAmplitude = FMath::Clamp(Amplitude, 0, 1);
		ApplyUserSettings();
		MarkUserSettingsDirty();		
	}
}

void UGearGameInstance::SetQualityLevel(int32 Level)
{
	if (IsValid(GearSave))
	{
		GearSave->QualityLevel = FMath::Clamp(Level, 1, 4);
		ApplyUserSettings();
		MarkUserSettingsDirty();
	}
}

void UGearGameInstance::SetLanguage(ELanguageOption Language)
{
	if (IsValid(GearSave))
	{
		GearSave->Language = Language;
		ApplyUserSettings();
		MarkUserSettingsDirty();
	}
}

float UGearGameInstance::GetSoundAmplitude()
{
	if (IsValid(GearSave))
	{
		return GearSave->SoundAmplitude;
	}

	return 0.5f;
}

float UGearGameInstance::GetMusicAmplitude()
{
	if (IsValid(GearSave))
	{
		return GearSave->MusicAmplitude;
	}

	return 0.5f;
}

int32 UGearGameInstance::GetQualityLevel()
{
	if (IsValid(GearSave))
	{
		return GearSave->QualityLevel;
	}

	return 2;
}

ELanguageOption UGearGameInstance::GetLanguage()
{
	if (IsValid(GearSave))
	{
		return GearSave->Language;
	}

	return ELanguageOption::Farsi;
}

void UGearGameInstance::SetDisconnectionReason(EPlayerDisconnectionReason InDisconnectionReason)
{
	DisconnectionReason = InDisconnectionReason;
}

EPlayerDisconnectionReason UGearGameInstance::GetDisconnectionReason()
{
	return DisconnectionReason;
}

void UGearGameInstance::ChangePerformanceSettings(bool bEnabingLoadingScreen)
{
	UGameInstance* LocalGameInstance = this;
	UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient();

	FShaderPipelineCache::SetBatchMode(bEnabingLoadingScreen ? FShaderPipelineCache::BatchMode::Fast : FShaderPipelineCache::BatchMode::Background);

	// Don't bother drawing the 3D world while we're loading
	GameViewportClient->bDisableWorldRendering = bEnabingLoadingScreen;

	// Make sure to prioritize streaming in levels if the loading screen is up
	if (UWorld* ViewportWorld = GameViewportClient->GetWorld())
	{
		if (AWorldSettings* WorldSettings = ViewportWorld->GetWorldSettings(false, false))
		{
			WorldSettings->bHighPriorityLoadingLocal = bEnabingLoadingScreen;
		}
	}

	if (bEnabingLoadingScreen)
	{
		// Set a new hang detector timeout multiplier when the loading screen is visible.
		double HangDurationMultiplier = 5;

		FThreadHeartBeat::Get().SetDurationMultiplier(HangDurationMultiplier);

		// Do not report hitches while the loading screen is up
		FGameThreadHitchHeartBeat::Get().SuspendHeartBeat();
	}
	else
	{
		// Restore the hang detector timeout when we hide the loading screen
		FThreadHeartBeat::Get().SetDurationMultiplier(1.0);

		// Resume reporting hitches now that the loading screen is down
		FGameThreadHitchHeartBeat::Get().ResumeHeartBeat();
	}
}