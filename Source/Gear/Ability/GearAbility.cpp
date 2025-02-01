// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GearAbility.h"
#include "Vehicle/GearVehicle.h"
#include "Vehicle/GearDriver.h"
#include "Net/UnrealNetwork.h"

AGearAbility::AGearAbility()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

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

void AGearAbility::Destroyed()
{
	if (HasAuthority() && IsValid(OwningVehice))
	{
		OwningVehice->ClearAbility();
	}

	Super::Destroyed();
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

EAbilityType AGearAbility::GetAbilityType()
{
	return AbilityType;
}

void AGearAbility::OnRep_OwningVehicle()
{
	UAnimInstance* AnimInstance = GetDriverBodyAnimInstance();
	if (IsValid(AnimInstance) && IsValid(ItemGrabMontage))
	{
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AGearAbility::OnMontageNotify);
		AnimInstance->Montage_Play(ItemGrabMontage);
	}
}

void AGearAbility::ActivateAbility()
{

}

float AGearAbility::GetWidgetValue()
{
	return 1.0f;
}

void AGearAbility::OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	
}

UAnimInstance* AGearAbility::GetDriverBodyAnimInstance()
{
	USkeletalMeshComponent* DriverBody = IsValid(OwningVehice) ? OwningVehice->GetDriverBody() : nullptr;
	UAnimInstance* AnimInstance = IsValid(DriverBody) ? DriverBody->GetAnimInstance() : nullptr;

	return AnimInstance;
}

bool AGearAbility::CanActivate()
{
	return true;
}