// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/PlaceableSpawnPoint.h"
#include "GameFramework/GameState.h"

APlaceableSpawnPoint::APlaceableSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;

	//FMath::RandInit();
	PreviewRotaionOffset = FMath::FRandRange(0.0f, 360.0f);
}

void APlaceableSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlaceableSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator Rotator = Rotator.ZeroRotator;
	Rotator.Yaw = GetWorld()->GetGameState()->GetServerWorldTimeSeconds() * PreviewRotationSpeed + PreviewRotaionOffset;

	SetActorRotation(Rotator);
}