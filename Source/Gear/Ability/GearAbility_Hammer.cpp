// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability/GearAbility_Hammer.h"

#define HAND_SOCKET_NAME TEXT("Hand")

AGearAbility_Hammer::AGearAbility_Hammer()
{
	HammerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HammerMesh"));
	HammerMesh->SetupAttachment(Root);


}

void AGearAbility_Hammer::OnRep_OwningVehicle()
{
	Super::OnRep_OwningVehicle();

	FTransform HandSocketTransform = IsValid(HammerMesh) ? HammerMesh->GetSocketTransform(HAND_SOCKET_NAME, RTS_Component) : FTransform::Identity;


}