// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GearAbility.h"
#include "Net/UnrealNetwork.h"

AGearAbility::AGearAbility()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	AbilityIcon = nullptr;

	bReplicates = true;
	bAlwaysRelevant = true;
}

void AGearAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearAbility, OwningVehice);
}

void AGearAbility::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGearAbility::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGearAbility::SetOwningVehicle(AGearVehicle* InOwningVehice)
{
	if (HasAuthority())
	{
		OwningVehice = InOwningVehice;
		OnRep_OwningVehicle();
	}
}

void AGearAbility::OnRep_OwningVehicle()
{

}