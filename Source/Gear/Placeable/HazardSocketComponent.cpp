// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/HazardSocketComponent.h"

UHazardSocketComponent::UHazardSocketComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UHazardSocketComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UHazardSocketComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}