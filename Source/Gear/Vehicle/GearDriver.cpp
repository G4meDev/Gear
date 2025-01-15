// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/GearDriver.h"

#define ITEM_SOCKET_NAME TEXT("hand_r_ItemSocket")

AGearDriver::AGearDriver()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(Root);

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(Root);
}

void AGearDriver::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGearDriver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

USkeletalMeshComponent* AGearDriver::GetHeadMesh()
{
	return HeadMesh;
}