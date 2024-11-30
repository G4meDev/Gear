// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearPlaceable.h"

AGearPlaceable::AGearPlaceable()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGearPlaceable::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGearPlaceable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGearPlaceable::RoundReset()
{
	RoundRestBlueprintEvent();
}

