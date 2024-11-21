// Fill out your copyright notice in the Description page of Project Settings

#include "GameFramework/GearGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GearSaveGame.h"
#include "GameFramework/GearTypes.h"

#define SAVE_SLOT_NAME "Save"
#define SAVE_USER_INDEX 0

void UGearGameInstance::Init()
{
	Super::Init();

	InitPersistantData();
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
	if (!GearSave)
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

FString UGearGameInstance::ValidtePlayerNameChange(const FString& NewName, const FString& OldName)
{
	//TODO: name validation

	return NewName;
}

FString UGearGameInstance::GetPlayerName() const 
{
	if (GearSave)
	{
		return GearSave->PlayerName;
	}

	return DEFAULT_PLAYER_NAME;
}

FString UGearGameInstance::TryChangePlayerName(const FString& NewName)
{
	if (GearSave)
	{
		FString ValidatedName = ValidtePlayerNameChange(NewName, GetPlayerName());
		GearSave->PlayerName = ValidatedName;
		TrySaveGameAsync();
	}

	return GetPlayerName();
}

#undef SAVE_SLOT_NAME
#undef SAVE_USER_INDEX