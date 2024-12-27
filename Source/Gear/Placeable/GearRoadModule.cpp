// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearRoadModule.h"
#include "GameFramework/GearGameState.h"
#include "Placeable/PlaceableSocket.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
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

	MainCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("MainCollider"));
	MainCollider->SetupAttachment(Root);
	MainCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MainCollider->ShapeColor = FColor::Blue;

	ExtentCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("ExtentCollider"));
	ExtentCollider->SetupAttachment(Root);
	ExtentCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExtentCollider->ShapeColor = FColor::Yellow;

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

		UpdateSplineParameters();
		UpdateColliders();
	}
}

void AGearRoadModule::UpdateSplineParameters()
{
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

void AGearRoadModule::UpdateColliders()
{
	if (IsValid(RoadMesh) && IsValid(MainCollider) && IsValid(ExtentCollider))
	{
		FVector MinBound;
		FVector MaxBound;
		RoadMesh->GetLocalBounds(MinBound, MaxBound);

		FVector BoundOrigin = (MinBound + MaxBound) / 2;
		//BoundOrigin *= RoadMesh->GetRelativeScale3D();
		BoundOrigin = RoadMesh->GetRelativeTransform().TransformPosition(BoundOrigin);

		FVector BoundExtent = (MaxBound - MinBound) / 2;
		BoundExtent += MainColliderPadding;

		MainCollider->SetRelativeLocation(BoundOrigin);
		MainCollider->SetBoxExtent(BoundExtent);


		BoundOrigin = RoadEndSocket->GetRelativeTransform().TransformPosition(FVector(ExtentColliderSize.X, 0, 0));
		BoundExtent = ExtentColliderSize;

		ExtentCollider->SetRelativeLocation(BoundOrigin);
		ExtentCollider->SetRelativeRotation(RoadEndSocket->GetRelativeRotation());
		ExtentCollider->SetBoxExtent(BoundExtent);
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

// -----------------------------------------------------------------------------

void AGearRoadModule::SetMainColliderEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		MainCollider->SetCollisionProfileName(TEXT("RoadBoundCollider"));
	}
	else
	{
		MainCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AGearRoadModule::OnIdle_Start()
{
	Super::OnIdle_Start();

	SetMainColliderEnabled(true);
}

void AGearRoadModule::OnIdle_End()
{
	Super::OnIdle_End();

	SetMainColliderEnabled(false);
}