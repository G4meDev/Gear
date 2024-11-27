// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GearTypes.h"
#include "GearPlayerState.generated.h"

class AGearHazardActor;

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

	void SetSelectedHazard(AGearHazardActor* Hazard);

	UPROPERTY(Replicated)
	AGearHazardActor* SelectedHazard;


	bool HasSelectedHazard() const;

protected:

	void OnRep_PlayerName() override;

	void CopyProperties(APlayerState* PlayerState) override;



};
