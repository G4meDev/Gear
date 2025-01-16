// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability/GearAbility_Hammer.h"
#include "Vehicle/GearVehicle.h"
#include "Vehicle/GearDriver.h"
#include "GameFramework/GearGameState.h"

#define HAND_SOCKET_NAME TEXT("Hand")
#define NOTIFY_ITEM_GRABBED_NAME TEXT("ItemGrabed")
//#define NOTIFY_ATTACK_HIT_NAME TEXT("AttackHit")

AGearAbility_Hammer::AGearAbility_Hammer()
{
	HammerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HammerMesh"));
	HammerMesh->SetupAttachment(Root);
	HammerMesh->SetCollisionProfileName(TEXT("NoCollision"));
	HammerMesh->SetHiddenInGame(true);

	AttackHitDelay = 0.12f;
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

	if (CanActivate())
	{
		PlayAttackMonrage();

		if (HasAuthority())
		{
			Activate_Multi(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());

			GetWorld()->GetTimerManager().SetTimer(AttackHitTimerHandle, FTimerDelegate::CreateUObject(this, &AGearAbility_Hammer::AttackHit), AttackHitDelay, false);
		}
	}
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

void AGearAbility_Hammer::Activate_Multi_Implementation(float ActivationTime)
{
	if (HasAuthority())
	{
		
	}
	else
	{
		if (!OwningVehice->IsLocallyControlled())
		{
			PlayAttackMonrage();
		}

		float ClientDelay = GetWorld()->GetGameState()->GetServerWorldTimeSeconds() - ActivationTime;
		float TimerDuration = AttackHitDelay - ClientDelay;

		if (TimerDuration > 0)
		{
			GetWorld()->GetTimerManager().SetTimer(AttackHitTimerHandle, FTimerDelegate::CreateUObject(this, &AGearAbility_Hammer::AttackHit), TimerDuration, false);
		}
	}
}

void AGearAbility_Hammer::PlayAttackMonrage()
{
	UAnimInstance* AnimInstance = GetDriverBodyAnimInstance();
	if (IsValid(AnimInstance) && IsValid(AttackMontage))
	{
		AnimInstance->Montage_Play(AttackMontage);
	}
}

void AGearAbility_Hammer::AttackHit()
{
	DrawDebugSphere(GetWorld(), GetActorLocation(), 50.0f, 8, FColor::Black, false, 0.1f);
}