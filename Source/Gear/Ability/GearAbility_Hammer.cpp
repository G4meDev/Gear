// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability/GearAbility_Hammer.h"
#include "Vehicle/GearVehicle.h"
#include "Vehicle/GearDriver.h"

#define HAND_SOCKET_NAME TEXT("Hand")
#define NOTIFY_ITEM_GRABBED_NAME TEXT("ItemGrabed")

AGearAbility_Hammer::AGearAbility_Hammer()
{
	HammerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HammerMesh"));
	HammerMesh->SetupAttachment(Root);
	HammerMesh->SetCollisionProfileName(TEXT("NoCollision"));
	HammerMesh->SetHiddenInGame(true);

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

void AGearAbility_Hammer::ActivateAbility()
{
	Super::ActivateAbility();


}

void AGearAbility_Hammer::OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::OnMontageNotify(NotifyName, BranchingPointPayload);

	if (NotifyName == NOTIFY_ITEM_GRABBED_NAME)
	{
		HammerMesh->SetHiddenInGame(false);
	}
}

bool AGearAbility_Hammer::CanActivate() const
{
	return Super::CanActivate();
}