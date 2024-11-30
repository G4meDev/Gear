
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystems/RoundResetable.h"
#include "GameFramework/GearTypes.h"
#include "GearPlaceable.generated.h"

class AGearPlayerState;

UCLASS(Blueprintable)
class GEAR_API AGearPlaceable : public AActor, public IRoundResetable
{
	GENERATED_BODY()
	
public:	
	AGearPlaceable();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void RoundReset() override;

	virtual void SetPreview() {};

	virtual void SetSelectedBy(AGearPlayerState* Player) {};

	UFUNCTION(BlueprintImplementableEvent)
	void RoundRestBlueprintEvent();


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPlaceableType Type;


	UPROPERTY(ReplicatedUsing=OnRep_OwningPlayer)
	AGearPlayerState* OwningPlayer;

	bool HasOwningPlayer() const;

	UFUNCTION()
	void OnRep_OwningPlayer();

protected:
	virtual void BeginPlay() override;
};