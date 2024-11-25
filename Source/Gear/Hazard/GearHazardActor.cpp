// Fill out your copyright notice in the Description page of Project Settings.


#include "Hazard/GearHazardActor.h"
#include "Net/UnrealNetwork.h"

AGearHazardActor::AGearHazardActor()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	PreviewRotaionOffset = FMath::FRandRange(0.0f, 360.0f);
	HazardState = EHazardState::Idle;
}

void AGearHazardActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearHazardActor, HazardState);
}

void AGearHazardActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (HazardState == EHazardState::Preview)
	{
		GoPreview();
	}
}

void AGearHazardActor::SetHazardState(EHazardState State)
{
	HazardState = State;
}

void AGearHazardActor::OnRep_HazardState(EHazardState OldValue)
{
	if (OldValue != HazardState)
	{
		if (HazardState == EHazardState::Preview)
		{
			GoPreview();
		}
	}
}

void AGearHazardActor::GoPreview()
{

}

void AGearHazardActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// in preview mode rotate locally
	if (HazardState == EHazardState::Preview)
	{
		FRotator Rotator = Rotator.ZeroRotator;
		Rotator.Yaw = GetWorld()->GetTimeSeconds() * PreviewRotationSpeed + PreviewRotaionOffset;

		SetActorRotation(Rotator);
	}

}