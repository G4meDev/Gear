// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GearTypes.h"
#include "LobbyPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCustomizationColorChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCustomizationHeadChanged);

/**
 * 
 */
UCLASS()
class GEAR_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	
	ALobbyPlayerState();
	virtual void PostNetInit() override;
	void BeginPlay();
	void Destroyed() override;

	void OnRep_PlayerName() override;
	void CopyProperties(APlayerState* PlayerState) override;

	UFUNCTION()
	void OnRep_PlayerJoinTime();

	UPROPERTY(ReplicatedUsing=OnRep_PlayerJoinTime)
	float PlayerJoinTime;

	UFUNCTION()
	void OnRep_PlayerCustomization(FPlayerCustomization OldCustomization);

	UPROPERTY(ReplicatedUsing=OnRep_PlayerCustomization)
	FPlayerCustomization PlayerCusstomization;

	UPROPERTY(Replicated)
	bool bReady;

	friend class ALobbyGameState;

public:

	UPROPERTY(ReplicatedUsing=OnRep_ColorCode, BlueprintReadWrite, EditAnywhere)
	EPlayerColorCode ColorCode;

	UFUNCTION()
	virtual void OnRep_ColorCode();


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FColor PlayerColor;

	float GetPlayerJoinTime() const;

	static bool SortJoinTimePredicate(const ALobbyPlayerState& P1, const ALobbyPlayerState& P2);


	UPROPERTY(BlueprintAssignable)
	FOnPlayerCustomizationColorChanged OnPlayerCustomizationColorChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerCustomizationHeadChanged OnPlayerCustomizationHeadChanged;

	UFUNCTION(BlueprintPure)
	FPlayerCustomization GetPlayerCustomization();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetPlayerCustomization_Server(const FPlayerCustomization& InPlayerCustomization);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetPlayerHeadType_Server(EDriverHead HeadType);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetPlayerTricycleColor_Server(EPlayerColorCode Color);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetPlayerClothColor_Server(EPlayerColorCode Color);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetPlayerPantColor_Server(EPlayerColorCode Color);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetPlayerHandColor_Server(EPlayerColorCode Color);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetPlayerShoeColor_Server(EPlayerColorCode Color);


	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetReady_Server(bool InbReady);

	UFUNCTION(BlueprintPure)
	bool IsReady();

protected:
};
