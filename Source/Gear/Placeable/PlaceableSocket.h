// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameFramework/GearTypes.h"
#include "PlaceableSocket.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEAR_API UPlaceableSocket : public USceneComponent
{
	GENERATED_BODY()

public:	

	UPlaceableSocket();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPlaceableSocketType SocketType;

	bool IsOccupied() const;

	void MarkOccupied();

	FTransform GetPlaceableSocketTransform();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(Replicated)
	bool Occupied;
};
