// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/GearDriver.h"
#include "Vehicle/GearVehicle.h"
#include "Vehicle/DriverHead.h"
#include "Ability/GearAbility.h"

#define ITEM_SOCKET_NAME TEXT("hand_r_ItemSocket")

#define MAT_NAME_CLOTH_COLOR TEXT("ClothColor")
#define MAT_NAME_PANT_COLOR TEXT("PantColor")
#define MAT_NAME_HAND_COLOR TEXT("HandColor")
#define MAT_NAME_SHOE_COLOR TEXT("ShoeColor")

AGearDriver::AGearDriver()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(Root);
	BodyMesh->bReceivesDecals = false;
}

void AGearDriver::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (IsValid(BodyMesh))
	{
		BodyMID = BodyMesh->CreateDynamicMaterialInstance(0);

	}


}

void AGearDriver::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGearDriver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGearDriver::Destroyed()
{
	DestroyDriverHead();
}

void AGearDriver::ChangeDriverHead(TSubclassOf<class ADriverHead> DriverHeadClass)
{
	DestroyDriverHead();

	if (IsValid(DriverHeadClass))
	{
		DriverHead = GetWorld()->SpawnActor<ADriverHead>(DriverHeadClass, GetActorTransform());
		
		if (IsValid(DriverHead))
		{
			DriverHead->SetOwningDriver(this);
			DriverHead->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}
}

void AGearDriver::SetOwningVehicle(AGearVehicle* InOwningVehicle)
{
	OwningVehicle = InOwningVehicle;
}

FName AGearDriver::GetItemSocketName()
{
	return ITEM_SOCKET_NAME;
}

USkeletalMeshComponent* AGearDriver::GetBodyMesh()
{
	return BodyMesh;
}

EAbilityType AGearDriver::GetAbilityType()
{
	return IsValid(OwningVehicle) && IsValid(OwningVehicle->GetAbility()) ? OwningVehicle->GetAbility()->GetAbilityType() : EAbilityType::None;
}

void AGearDriver::ChangePlayerCustomizationHead(const FPlayerCustomization& PlayerCustomization)
{
	ChangeDriverHead(PlayerCustomization.HeadType.Class);
}

void AGearDriver::ChangePlayerCustomizationColor(const FPlayerCustomization& PlayerCustomization)
{
	if (IsValid(BodyMID))
	{
		BodyMID->SetVectorParameterValue(MAT_NAME_CLOTH_COLOR, PlayerCustomization.ClothColor.LinearColor);
		BodyMID->SetVectorParameterValue(MAT_NAME_PANT_COLOR, PlayerCustomization.PantColor.LinearColor);
		BodyMID->SetVectorParameterValue(MAT_NAME_HAND_COLOR, PlayerCustomization.HandColor.LinearColor);
		BodyMID->SetVectorParameterValue(MAT_NAME_SHOE_COLOR, PlayerCustomization.ShoeColor.LinearColor);
	}
}

void AGearDriver::DestroyDriverHead()
{
	if (IsValid(DriverHead))
	{
		DriverHead->Destroy();
		DriverHead = nullptr;
	}	
}