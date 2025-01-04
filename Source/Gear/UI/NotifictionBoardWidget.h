// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GearTypes.h"
#include "NotifictionBoardWidget.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UNotifictionBoardWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent)
	void NotifyPlayerPlaced(class AGearPlayerState* Player, TSubclassOf<class AGearPlaceable> Placeable);

	UFUNCTION(BlueprintImplementableEvent)
	void NotifyElimination(class AGearPlayerState* PlayerState, EElimanationReason ElimanationReason);

	UFUNCTION(BlueprintImplementableEvent)
	void NotifyReachedCheckpoint(AGearPlayerState* PlayerState, class ACheckpoint* Checkpoint, int32 Position);
};