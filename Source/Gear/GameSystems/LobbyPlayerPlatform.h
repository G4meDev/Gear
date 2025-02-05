// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyPlayerPlatform.generated.h"

UCLASS()
class GEAR_API ALobbyPlayerPlatform : public AActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* DriverSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMeshComponent* Vehicle; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<class AGearDriver> DriverClass;

	UPROPERTY()
	class AGearDriver* Driver;

	UPROPERTY()
	class ALobbyPlayerState* OwningPlayer;

	UPROPERTY()
	UMaterialInstanceDynamic* VehicleMID;

	friend class ALobbyGameState;

public:	
	ALobbyPlayerPlatform();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	void ClearPlatform();

	class ALobbyPlayerState* GetOwningPlayer();

protected:

	void SetOwningPlayer(class ALobbyPlayerState* InOwningPlayer);

	UFUNCTION()
	void PlayerCustomizationChanged();
};