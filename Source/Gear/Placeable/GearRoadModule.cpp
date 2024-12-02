// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearRoadModule.h"
#include "Placeable/PlaceableSocket.h"
#include "Net/UnrealNetwork.h"

AGearRoadModule::AGearRoadModule()
{
	PrimaryActorTick.bCanEverTick = true;

	RoadEndSocket = CreateDefaultSubobject<UPlaceableSocket>(TEXT("EndSocket"));
	RoadEndSocket->SetupAttachment(Root);

	bFliped = false;
	PreviewScale = 0.1f;
}

void AGearRoadModule::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

// void AGearRoadModule::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
// 
// }

void AGearRoadModule::BeginPlay()
{
	Super::BeginPlay();


}

void AGearRoadModule::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}

void AGearRoadModule::SetPreview()
{
	Super::SetPreview();
}

void AGearRoadModule::SetSelectedBy(AGearBuilderPawn* Player)
{
	Super::SetSelectedBy(Player);
}

bool AGearRoadModule::IsFlipable()
{
	return true;
}
