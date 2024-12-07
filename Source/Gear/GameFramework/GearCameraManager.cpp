// Fill out your copyright notice in the Description page of Project Settings.


#include "GearCameraManager.h"
#include "GameSystems/TrackSpline.h"
#include "Placeable/GearRoadModule.h"

void AGearCameraManager::RoadModuleStackChanged(const TArray<AGearRoadModule*> RoadModulesStack)
{
	if (IsValid(TrackSpline))
	{
		TrackSpline->RoadModuleStackChanged(RoadModulesStack);
	}
}

void AGearCameraManager::BeginPlay()
{
	Super::BeginPlay();

}

void AGearCameraManager::InitializeFor(class APlayerController* PC)
{
	Super::InitializeFor(PC);

	if (GetOwningPlayerController()->IsLocalController())
	{
		TrackSpline = GetWorld()->SpawnActor<ATrackSpline>(ATrackSpline::StaticClass(), FTransform::Identity);
		check(TrackSpline);
	}
}

void AGearCameraManager::Destroyed()
{
	Super::Destroyed();

	if (GetOwningPlayerController()->IsLocalController())
	{
		if (IsValid(TrackSpline))
		{
			TrackSpline->Destroy();
		}
	}
}