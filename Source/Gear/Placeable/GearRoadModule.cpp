// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearRoadModule.h"
#include "Placeable/PlaceableSocket.h"

AGearRoadModule::AGearRoadModule()
{
	PrimaryActorTick.bCanEverTick = true;

	RoadEndSocket = CreateDefaultSubobject<UPlaceableSocket>(TEXT("EndSocket"));
	RoadEndSocket->SetupAttachment(Root);

	PreviewScale = 0.1f;
}

void AGearRoadModule::PostInitializeComponents()
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

void AGearRoadModule::BeginPlay()
{
	Super::BeginPlay();


}

void AGearRoadModule::SetPreview()
{
	Super::SetPreview();
}

void AGearRoadModule::SetSelectedBy(AGearPlayerState* Player)
{
	Super::SetSelectedBy(Player);
}

void AGearRoadModule::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}