// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/DriverHead.h"

ADriverHead::ADriverHead()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(Root);
	HeadMesh->bReceivesDecals = false;
}

void ADriverHead::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADriverHead::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

class AGearDriver* ADriverHead::GetOwningDriver()
{
	return OwningDriver;
}

void ADriverHead::SetOwningDriver(class AGearDriver* InOwningDriver)
{
	OwningDriver = InOwningDriver;
	OnOwningDriverChanged.Broadcast();
}