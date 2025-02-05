// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/LobbyPlayerPlatform.h"
#include "GameFramework/LobbyPlayerState.h"
#include "Vehicle/GearVehicle.h"
#include "Vehicle/GearDriver.h"



ALobbyPlayerPlatform::ALobbyPlayerPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Vehicle = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Vehicle"));
	Vehicle->SetupAttachment(Root);
	Vehicle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Vehicle->SetHiddenInGame(true);
}

void ALobbyPlayerPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALobbyPlayerPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALobbyPlayerPlatform::Destroyed()
{
	ClearPlatform();
	
	Super::Destroyed();
}

void ALobbyPlayerPlatform::ClearPlatform()
{
	Vehicle->SetHiddenInGame(true);

	if (IsValid(Driver))
	{
		Driver->Destroy();
	}
}

class ALobbyPlayerState* ALobbyPlayerPlatform::GetOwningPlayer()
{
	return OwningPlayer;
}

void ALobbyPlayerPlatform::SetOwningPlayer(class ALobbyPlayerState* InOwningPlayer)
{
	OwningPlayer = InOwningPlayer;

	if (IsValid(OwningPlayer))
	{
		Vehicle->SetHiddenInGame(false);
	}

	else
	{
		ClearPlatform();
	}
}