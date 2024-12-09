// Fill out your copyright notice in the Description page of Project Settings.


#include "GearCameraManager.h"
#include "GameFramework/GearGameState.h"
#include "Vehicle/GearVehicle.h"
#include "GameSystems/TrackSpline.h"


void AGearCameraManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AGearCameraManager::BeginPlay()
{
	Super::BeginPlay();

}

void AGearCameraManager::InitializeFor(class APlayerController* PC)
{
	Super::InitializeFor(PC);

	GearGameState = GetWorld()->GetGameState<AGearGameState>();
}

void AGearCameraManager::Destroyed()
{
	Super::Destroyed();


}