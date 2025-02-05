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
	USkeletalMeshComponent* Vehicle; 

	UPROPERTY()
	class AGearDriver* Driver;

	UPROPERTY()
	class ALobbyPlayerState* OwningPlayer;

	void SetOwningPlayer(class ALobbyPlayerState* InOwningPlayer);

	friend class ALobbyGameState;

public:	
	ALobbyPlayerPlatform();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	void ClearPlatform();

	class ALobbyPlayerState* GetOwningPlayer();

protected:

};