// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SelectionPlatform.generated.h"

UCLASS()
class GEAR_API ASelectionPlatform : public AActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UStaticMeshComponent* PlatformMesh;

public:	
	ASelectionPlatform();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
};
