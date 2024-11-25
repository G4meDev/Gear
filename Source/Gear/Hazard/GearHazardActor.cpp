// Fill out your copyright notice in the Description page of Project Settings.


#include "Hazard/GearHazardActor.h"

AGearHazardActor::AGearHazardActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

void AGearHazardActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGearHazardActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

