// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Online/LanBeacon.h"
#include "GameFramework/GearTypes.h"
#include "Delegates/DelegateCombinations.h"
#include "NetworkDiscoveryActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFoundHostDelegate, const FGearHostInfo&, HostInfo);

UCLASS()
class GEAR_API ANetworkDiscoveryActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ANetworkDiscoveryActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void StartDiscovery(bool bHost = true);

	UFUNCTION(BlueprintCallable)
	void StopDiscovery();

	FLANSession LanSession;

	void OnValidQueryPacket(uint8* Bytes, int32 Length, uint64 Nonce);

	void OnValidResponsePacket(uint8* Bytes, int32 Length);
	void OnSearchTimeout();

	UPROPERTY(BlueprintAssignable)
	FOnFoundHostDelegate OnFoundHostDelegate;
};
