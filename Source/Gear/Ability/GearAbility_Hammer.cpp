// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability/GearAbility_Hammer.h"
#include "Vehicle/GearVehicle.h"
#include "Vehicle/GearDriver.h"
#include "GameSystems/GearStatics.h"
#include "Components/DecalComponent.h"
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

	AttackRangeDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("AttckRangeDecal"));
	AttackRangeDecal->SetupAttachment(Root);
	AttackRangeDecal->SetHiddenInGame(true);

	AttackHitDelay = 0.12f;
	AttackInnerRadius = 256.0f;
	AttackOuterRadius = 512.0f;
	ImpulseStrength = 1000000.0f;
	ForceZOffset = -100.0f;
	VelocityReductionRatio = 0.7f;
}

void AGearAbility_Hammer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(OwningVehice))
	{
		FQuat Quat = OwningVehice->GetActorTransform().TransformRotation(FQuat(FVector::RightVector, PI/2));

		AttackRangeDecal->SetWorldLocationAndRotation(OwningVehice->GetActorLocation(), Quat.Rotator());
	}
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

	Activate_Server();
}

void AGearAbility_Hammer::OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::OnMontageNotify(NotifyName, BranchingPointPayload);

	if (NotifyName == NOTIFY_ITEM_GRABBED_NAME)
	{
		ShowHammer();
	}

	
}

bool AGearAbility_Hammer::CanActivate() const
{
	return Super::CanActivate();
}

void AGearAbility_Hammer::ShowHammer()
{
	HammerMesh->SetHiddenInGame(false);

	if (IsValid(OwningVehice) && OwningVehice->IsLocallyControlled())
	{
		AttackRangeDecal->SetHiddenInGame(false);	
	}
}

void AGearAbility_Hammer::Activate_Server_Implementation()
{
	if (CanActivate())
	{
		GetWorld()->GetTimerManager().SetTimer(AttackHitTimerHandle, FTimerDelegate::CreateUObject(this, &AGearAbility_Hammer::AttackHit), AttackHitDelay, false);
		Activate_Multi();
	}
}

void AGearAbility_Hammer::Activate_Multi_Implementation()
{
	ShowHammer();
	PlayAttackMontage();

	if (IsValid(AttackEffectClass) && IsValid(OwningVehice))
	{
		GetWorld()->SpawnActor<AActor>(AttackEffectClass, OwningVehice->GetActorLocation(), OwningVehice->GetActorRotation());
	}
}

void AGearAbility_Hammer::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetDriverBodyAnimInstance();
	if (IsValid(AnimInstance) && IsValid(AttackMontage))
	{
		AnimInstance->Montage_Play(AttackMontage);
	}
}

void AGearAbility_Hammer::AttackHit()
{
	if (!OwningVehice)
	{
		return;
	}

	FVector AttackLocation = OwningVehice->GetActorLocation();

	DrawDebugSphere(GetWorld(), AttackLocation, AttackInnerRadius, 8, FColor::Blue, false, 0.1f);
	DrawDebugSphere(GetWorld(), AttackLocation, AttackOuterRadius, 8, FColor::Red, false, 0.1f);

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(OwningVehice);
	TArray<AGearVehicle*> InRangeVehicles;
	UGearStatics::SphereOverlapForVehicles(this, InRangeVehicles, AttackLocation, AttackOuterRadius, IgnoreActors);

	for (AGearVehicle* Vehicle : InRangeVehicles)
	{
		if (IsValid(Vehicle) && Vehicle->IsOnGround())
		{
			float Distance = FVector::Distance(AttackLocation, Vehicle->GetActorLocation());
			float Alpha = (Distance - AttackInnerRadius) / (AttackOuterRadius - AttackInnerRadius);
			Alpha = 1 - FMath::Clamp(Alpha, 0, 1);

			FVector ForceDirection = (Vehicle->GetActorLocation() - AttackLocation - FVector::UpVector * ForceZOffset);
			ForceDirection.Normalize();

			Vehicle->ReduceVelocityBeRatio(VelocityReductionRatio);
			Vehicle->GetMesh()->AddImpulse(ForceDirection * ImpulseStrength * Alpha);
		}
	}
}