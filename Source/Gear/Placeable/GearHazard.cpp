// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearHazard.h"
#include "Net/UnrealNetwork.h"


AGearHazard::AGearHazard()
{
	PrimaryActorTick.bCanEverTick = true;


}

void AGearHazard::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

// void AGearHazard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
// 
// 
// 	
// }

void AGearHazard::BeginPlay()
{
	Super::BeginPlay();


}

void AGearHazard::SetPreview()
{
	Super::SetPreview();
}

void AGearHazard::SetSelectedBy(AGearBuilderPawn* Player)
{
	Super::SetSelectedBy(Player);
}

void AGearHazard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}