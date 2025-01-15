// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability/GearAbility_Hammer.h"
#include "Vehicle/GearVehicle.h"
#include "Vehicle/GearDriver.h"

#define HAND_SOCKET_NAME TEXT("Hand")

AGearAbility_Hammer::AGearAbility_Hammer()
{
	HammerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HammerMesh"));
	HammerMesh->SetupAttachment(Root);
	HammerMesh->SetCollisionProfileName(TEXT("NoCollision"));


}

void AGearAbility_Hammer::OnRep_OwningVehicle()
{
	Super::OnRep_OwningVehicle();

	AGearDriver* Driver = OwningVehice->GetDriver();

	if (IsValid(Driver) && IsValid(Driver->GetBodyMesh()) && Driver->GetBodyMesh()->DoesSocketExist(Driver->GetItemSocketName()))
	{
		AttachToComponent(Driver->GetBodyMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Driver->GetItemSocketName());
		FTransform HandSocketTransform = IsValid(HammerMesh) ? HammerMesh->GetSocketTransform(HAND_SOCKET_NAME, RTS_Component) : FTransform::Identity;
		SetActorRelativeLocation(-HandSocketTransform.GetLocation());
		SetActorRelativeRotation(HandSocketTransform.Rotator().GetInverse());
	}
	


}