// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HazardSocketComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEAR_API UHazardSocketComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UHazardSocketComponent();

protected:
	virtual void BeginPlay() override;



public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
};