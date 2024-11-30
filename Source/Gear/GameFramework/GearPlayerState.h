// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GearTypes.h"
#include "GearPlayerState.generated.h"

class AGearPlaceable;

/**
 * 
 */
UCLASS()
class GEAR_API AGearPlayerState : public APlayerState
{
	GENERATED_BODY()


public:

	AGearPlayerState();

	UPROPERTY(ReplicatedUsing=OnRep_ColorCode, BlueprintReadWrite, EditAnywhere)
	EPlayerColorCode ColorCode;

	UFUNCTION()
	virtual void OnRep_ColorCode();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FColor PlayerColor;

	void SetSelectedHazard(AGearPlaceable* Placeable);

	UPROPERTY(Replicated)
	AGearPlaceable* SelectedPlaceable;


	bool HasSelectedHazard() const;

protected:

	void OnRep_PlayerName() override;

	void CopyProperties(APlayerState* PlayerState) override;



};
