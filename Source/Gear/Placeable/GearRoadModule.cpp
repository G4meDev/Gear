// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearRoadModule.h"
#include "Placeable/PlaceableSocket.h"
#include "Net/UnrealNetwork.h"

AGearRoadModule::AGearRoadModule()
{
	PrimaryActorTick.bCanEverTick = true;

	RoadEndSocket = CreateDefaultSubobject<UPlaceableSocket>(TEXT("EndSocket"));
	RoadEndSocket->SetupAttachment(Root);

	PreviewScale = 0.1f;
	bMirrorX = false;
}

void AGearRoadModule::PostInitializeComponents()
{
	Super::PostInitializeComponents();

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

void AGearRoadModule::SetSelectedBy(AGearBuilderPawn* Player)
{
	Super::SetSelectedBy(Player);
}

void AGearRoadModule::MoveToSocket(UPlaceableSocket* TargetSocket, bool InMirrorX)
{
	if (IsValid(TargetSocket))
	{
		bMirrorX = InMirrorX;

		FVector TargetLocation = TargetSocket->GetComponentLocation();
		FRotator TargetRotation = TargetSocket->GetComponentRotation();

		if (bMirrorX)
		{
			FRotator DeltaRotator = FRotator(0, 180, 0) - RoadEndSocket->GetRelativeRotation();
			TargetRotation = TargetSocket->GetComponentTransform().TransformRotation(DeltaRotator.Quaternion()).Rotator();

			FVector DeltaLocation = DeltaRotator.RotateVector(RoadEndSocket->GetRelativeLocation());
			TargetLocation = TargetSocket->GetComponentTransform().TransformPositionNoScale(-DeltaLocation);
		}

		SetActorLocationAndRotation(TargetLocation, TargetRotation);
	}
}
