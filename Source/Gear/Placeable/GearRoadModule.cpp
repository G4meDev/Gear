// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearRoadModule.h"
#include "GameFramework/GearGameState.h"
#include "Placeable/PlaceableSocket.h"
#include "Components/SplineComponent.h"
#include "Net/UnrealNetwork.h"

AGearRoadModule::AGearRoadModule()
{
	PrimaryActorTick.bCanEverTick = true;

	RoadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoadMesh"));
	RoadMesh->SetupAttachment(Root);
	RoadMesh->SetCollisionProfileName(TEXT("BlockAll"));

	RoadStartSocket = CreateDefaultSubobject<UPlaceableSocket>(TEXT("StartSocket"));
	RoadStartSocket->SetupAttachment(Root);

	RoadEndSocket = CreateDefaultSubobject<UPlaceableSocket>(TEXT("EndSocket"));
	RoadEndSocket->SetupAttachment(Root);

	RoadSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RoadSpline"));
	RoadSpline->SetupAttachment(Root);

	PreviewScale = 0.3f;
	bMirrorX = false;
}

void AGearRoadModule::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AGearRoadModule, bMirrorX, COND_InitialOnly);
}

void AGearRoadModule::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AGearRoadModule::BeginPlay()
{
	Super::BeginPlay();


}

#if WITH_EDITOR
void AGearRoadModule::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName ProperyName = PropertyChangedEvent.Property == nullptr ? NAME_None : PropertyChangedEvent.Property->GetFName();

	if (ProperyName == GET_MEMBER_NAME_CHECKED(AGearRoadModule, bDirty))
	{
		bDirty = false;
		RoadLength = RoadSpline->GetSplineLength();

		if (IsValid(RoadMesh) && IsValid(SelectionIndicator) && IsValid(PreviewRotationPivot))
		{
			FVector MinBound;
			FVector MaxBound;
			RoadMesh->GetLocalBounds(MinBound, MaxBound);
			
			FVector RotationPivotLocation = (MinBound + MaxBound) / 2.0f;
			RotationPivotLocation.Z = MinBound.Z + RotationPivotHeightOffset;

			PreviewRotationPivot->SetRelativeLocation(RotationPivotLocation);
		}
	}
}
#endif

void AGearRoadModule::PostNetInit()
{
	Super::PostNetInit();

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

void AGearRoadModule::MoveToSocket(const FTransform& TargetSocket, bool InMirrorX)
{
	bMirrorX = InMirrorX;

	FVector TargetLocation = TargetSocket.GetLocation();
	FRotator TargetRotation = TargetSocket.Rotator();

	FVector DebugStart = TargetLocation + FVector::UpVector * 20;
		
	if (bMirrorX)
	{
		FRotator DeltaRotator = FRotator(0, 180, 0) - RoadEndSocket->GetRelativeRotation();
		TargetRotation = TargetSocket.TransformRotation(DeltaRotator.Quaternion()).Rotator();

		FVector DeltaLocation = DeltaRotator.RotateVector(RoadEndSocket->GetRelativeTransform().GetLocation());
		TargetLocation = TargetSocket.TransformPositionNoScale(-DeltaLocation);
	}

	SetActorLocationAndRotation(TargetLocation, TargetRotation);
}

UPlaceableSocket* AGearRoadModule::GetAttachableSocket()
{
	return bMirrorX ? RoadStartSocket : RoadEndSocket;
}