// Fill out your copyright notice in the Description page of Project Settings.

#include "Placeable/SelectionPlatform.h"

ASelectionPlatform::ASelectionPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(Root);
	PlatformMesh->SetCollisionProfileName(TEXT("NoCollision"));

	bReplicates = true;
	bAlwaysRelevant = true;
}

void ASelectionPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASelectionPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}