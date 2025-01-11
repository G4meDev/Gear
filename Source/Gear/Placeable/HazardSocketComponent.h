// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameFramework/GearTypes.h"
#include "HazardSocketComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEAR_API UHazardSocketComponent : public USceneComponent
{
	GENERATED_BODY()

protected:


public:	

	UHazardSocketComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnRep_Occupied();

	bool IsOccupied() const;

	void MarkOccupied();



protected:
	
	UPROPERTY(ReplicatedUsing=OnRep_Occupied)
	bool bOccupied;

	UPROPERTY(ReplicatedUsing=OnRep_Occupied, EditDefaultsOnly)
	EHazardSocketType SocketType;
};