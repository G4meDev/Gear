// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameFramework/Geartypes.h"
#include "GearHUD.generated.h"

class AGearPlayerState;
class ACheckpoint;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllPlayersJoined);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoined, AGearPlayerState*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerQuit, AGearPlayerState*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnPlayerEliminated, AGearPlayerState*, Player, EElimanationReason, ElimanationReason, float, EliminationTime, int32, RemainingPlayersCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnReachedCheckpoint, AGearPlayerState*, Player, ACheckpoint*, Checkpoint, int32, Position, int32, AllCheckpointNum, float, ReachTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRaceStart, float, StartTime, bool, bWithCountDown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityStateChanged, class AGearAbility*, Ability);

/**
 * 
 */
UCLASS()
class GEAR_API AGearHUD : public AHUD
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category=Classes)
	TSubclassOf<UGearBaseWidget> ScreenMenuWidgetClass;
	UPROPERTY()
	UGearBaseWidget* ScreenMenuWidget;
	
	UPROPERTY(EditDefaultsOnly, Category=Classes)
	TSubclassOf<UGearBaseWidget> PauseWidgetClass;
	UPROPERTY()
	UGearBaseWidget* PauseWidget;

	UPROPERTY(EditDefaultsOnly, Category=Classes)
	TSubclassOf<UGearBaseWidget> NotifictionWidgetClass;
	UPROPERTY()
	UGearBaseWidget* NotifictionWidget;

	UPROPERTY(EditDefaultsOnly, Category=Classes)
	TSubclassOf<UGearBaseWidget> WaitingWidgetClass;
	UPROPERTY()
	UGearBaseWidget* WaitingWidget;

	UPROPERTY(EditDefaultsOnly, Category=Classes)
	TSubclassOf<UGearBaseWidget> VehicleInputWidgetClass;
	UPROPERTY()
	UGearBaseWidget* VehicleInputWidget;

	UPROPERTY(EditDefaultsOnly, Category=Classes)
	TSubclassOf<UGearBaseWidget> SelectingWidgetClass;
	UPROPERTY()
	UGearBaseWidget* SelectingWidget;

	UPROPERTY(EditDefaultsOnly, Category=Classes)
	TSubclassOf<UGearBaseWidget> PlacingWidgetClass;
	UPROPERTY()
	UGearBaseWidget* PlacingWidget;

	UPROPERTY(EditDefaultsOnly, Category=Classes)
	TSubclassOf<UGearBaseWidget> RacingWidgetClass;
	UPROPERTY()
	UGearBaseWidget* RacingWidget;

	UPROPERTY(EditDefaultsOnly, Category=Classes)
	TSubclassOf<UGearBaseWidget> ScoreboardWidgetClass;
	UPROPERTY()
	UGearBaseWidget* ScoreboardWidget;

	UPROPERTY(EditDefaultsOnly, Category=Classes)
	TSubclassOf<UGearBaseWidget> FinishboardWidgetClass;
	UPROPERTY()
	UGearBaseWidget* FinishboardWidget;

	bool bPaused;

public:

	AGearHUD();
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnAllPlayersJoined OnAllPlayersJoined;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnPlayerJoined OnPlayerJoined;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnPlayerQuit OnPlayerQuit;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnPlayerEliminated OnPlayerEliminated;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnReachedCheckpoint OnReachedCheckpoint;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnRaceStart OnRaceStart;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnAbilityStateChanged OnAbilityStateChanged;

	void AddVehicleInputWidget(class AGearVehicle* OwningVehicle);
	void RemoveVehicleInputWidget();

protected:
	TArray<class UGearBaseWidget*> WidgetStack;

	UFUNCTION(BlueprintCallable)
	void AddWidget(TSubclassOf<UGearBaseWidget> WidgetClass, UGearBaseWidget*& Widget, float InStartTime);

	UFUNCTION(BlueprintCallable)
	void RemoveWidget(UGearBaseWidget*& Widget);

	UFUNCTION(BlueprintCallable)
	void ReconstructWidgetsOrder();

	void SetVisilityOfWidgetsInStack(bool bVisible);

// -----------------------------------------------------------------------------------------

	void Waiting_Start(float StartTime);
	void Waiting_End();

	void Selecting_Start(float StartTime);
	void Selecting_End();

	void Placing_Start(float StartTime);
	void Placing_End();

	void Racing_Start(float StartTime);
	void Racing_End();

	void Scoreboard_Start(float StartTime);
	void Scoreboard_End();

	void Finishboard_Start(float StartTime);
	void Finishboard_End();

	UFUNCTION(BlueprintCallable)
	void Pause_Start(float StartTime);

	UFUNCTION(BlueprintCallable)
	void Pause_End();

	UFUNCTION(BlueprintPure)
	bool IsPaused();

	void AllPlayersJoined();
	void PlayerJoined(class AGearPlayerState* Player);
	void PlayerQuit(class AGearPlayerState* Player);

	void PlayerEliminated(AGearPlayerState* Player, EElimanationReason ElimanationReason, float EliminationTime, int32 RemainingPlayersCount);
	void ReachedCheckpoint(AGearPlayerState* Player, class ACheckpoint* Checkpoint, int32 Position, int32 AllCheckpointNum, float ReachTime);

	void RaceStart(float StartTime, bool bWithCountDown);

	void AbilityStateChanged(class AGearAbility* Ability);
	
	friend class AGearPlayerController;
	friend class AGearVehicle;
};