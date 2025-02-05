// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GearTypes.h"
#include "LobbyPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCustomizationChanged);

/**
 * 
 */
UCLASS()
class GEAR_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	
	ALobbyPlayerState();
	void BeginPlay();
	void Destroyed() override;

	void OnRep_PlayerName() override;
	void CopyProperties(APlayerState* PlayerState) override;

	UPROPERTY(Replicated)
	float PlayerJoinTime;

	UFUNCTION()
	void OnRep_PlayerCustomization();

	UPROPERTY(ReplicatedUsing=OnRep_PlayerCustomization)
	FPlayerCustomization PlayerCusstomization;

	friend class ALobbyGameState;

public:

	UPROPERTY(ReplicatedUsing=OnRep_ColorCode, BlueprintReadWrite, EditAnywhere)
	EPlayerColorCode ColorCode;

	UFUNCTION()
	virtual void OnRep_ColorCode();


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FColor PlayerColor;

	UFUNCTION(BlueprintPure)
	float GetPlayerJoinTime();

	UPROPERTY(BlueprintAssignable)
	FOnPlayerCustomizationChanged OnPlayerCustomizationChanged;

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

protected:
};
