// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability/GearAbility_Hammer.h"
#include "Vehicle/GearVehicle.h"
#include "Vehicle/GearDriver.h"
#include "GameSystems/GearStatics.h"
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
	AttackInnerRadius = 500.0f;
	AttackOuterRadius = 1000.0f;
	ImpulseStrength = 1000000.0f;
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
		PlayAttackMontage();

		Activate_Server();
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

void AGearAbility_Hammer::Activate_Server_Implementation()
{
	if (CanActivate())
	{
		FVector ActivationLocation = GetActorLocation();
		float ActivationTime = GetWorld()->GetTimeSeconds();

		float HitTime = ActivationTime + AttackHitDelay;

		UE_LOG(LogTemp, Warning, TEXT("SSSSSSSSSSSS : %f"), ActivationTime);

		Activate_Multi(HitTime, ActivationLocation);

		GetWorld()->GetTimerManager().SetTimer(AttackHitTimerHandle, FTimerDelegate::CreateUObject(this, &AGearAbility_Hammer::AttackHit, ActivationLocation), AttackHitDelay, false);
	}
}

void AGearAbility_Hammer::Activate_Multi_Implementation(float AttackHitTime, FVector Location)
{
	if (HasAuthority())
	{
		
	}
	else
	{
		if (!OwningVehice->IsLocallyControlled())
		{
			PlayAttackMontage();
		}

// 		UNetConnection* NetConnection = GetWorld()->GetGameState()->GetNetConnection();
// 		GetWorld()->
// 		APlayerState* a;
// 
// 		a->GetPingInMilliseconds()
// 
// 		float TimerDelay = AttackHitDelay - NetConnection->RTT
// 
// 		UE_LOG(LogTemp, Warning, TEXT("WWWWWWWWWWWWWWWWWWWW : %f"), TimerDelay);
// 
// 		if (TimerDelay > 0)
// 		{
// 			GetWorld()->GetTimerManager().SetTimer(AttackHitTimerHandle, FTimerDelegate::CreateUObject(this, &AGearAbility_Hammer::AttackHit, Location), TimerDelay, false);
// 		} 
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

void AGearAbility_Hammer::AttackHit(FVector Location)
{
	//if (HasAuthority())
	if (true)
	{
		FVector AttackLocation = Location;
		//FVector AttackLocation = GetActorLocation();

		DrawDebugSphere(GetWorld(), AttackLocation, AttackInnerRadius, 8, FColor::Blue, false, 0.1f);
		DrawDebugSphere(GetWorld(), AttackLocation, AttackOuterRadius, 8, FColor::Red, false, 0.1f);

		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(OwningVehice);
		TArray<AGearVehicle*> InRangeVehicles;
		UGearStatics::SphereOverlapForVehicles(this, InRangeVehicles, AttackLocation, AttackOuterRadius, IgnoreActors);

		for (AGearVehicle* Vehicle : InRangeVehicles)
		{
			if (IsValid(Vehicle))
			{
				Vehicle->GetMesh()->AddImpulse(FVector::UpVector * ImpulseStrength);
			}
		}
	}
}