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

void AGearRoadModule::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearRoadModule, bFliped);
}

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

void AGearRoadModule::SetSelectedBy(AGearPlayerState* Player)
{
	Super::SetSelectedBy(Player);
}

void AGearRoadModule::Flip()
{
	bFliped = !bFliped;

	if (bFliped)
	{
		SetActorRelativeScale3D(FVector(1, -1, 1));
	}
	else
	{
		SetActorRelativeScale3D(FVector::One());
	}
}