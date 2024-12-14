// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SpawnableSocket.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEAR_API USpawnableSocket : public USceneComponent
{
	GENERATED_BODY()

public:	
	USpawnableSocket();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AActor> SpawnClass;

protected:
	virtual void BeginPlay() override;	

};
