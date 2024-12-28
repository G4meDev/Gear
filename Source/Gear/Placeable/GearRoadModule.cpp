// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearRoadModule.h"
#include "GameFramework/GearGameState.h"
#include "GameSystems/GearStatics.h"
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

	bPrebuild = false;
	bPrebuildActive = false;
	TraceReult = ERoadModuleTraceResult::NotColliding;
}

// void AGearRoadModule::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
// 
// }

void AGearRoadModule::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AGearRoadModule::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < RoadMesh->GetNumMaterials(); i++)
	{
		UMaterialInstanceDynamic* MID = RoadMesh->CreateAndSetMaterialInstanceDynamic(i);
		check(MID);

		RoadMeshMaterials.Add(MID);
	}
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

	if (bPrebuild && bPrebuildActive)
	{
		ERoadModuleTraceResult NewResult = UGearStatics::TraceRoadModule(this, GetClass(), GetActorTransform());
		if (NewResult != TraceReult)
		{
			TraceReult = NewResult;
			OnTraceStateChanged(TraceReult);
		}
	}
}

void AGearRoadModule::SetPreview()
{
	Super::SetPreview();
}

void AGearRoadModule::SetSelectedBy(AGearBuilderPawn* Player)
{
	Super::SetSelectedBy(Player);
}

void AGearRoadModule::MoveToSocketTransform(const FTransform& TargetSocket)
{
	SetActorLocationAndRotation(TargetSocket.GetLocation(), TargetSocket.Rotator());
}

UPlaceableSocket* AGearRoadModule::GetAttachableSocket()
{
	return RoadEndSocket;
}

void AGearRoadModule::OnTraceStateChanged(ERoadModuleTraceResult Result)
{
	auto UpdateMaterials = [&](int32 State)
	{
		for (auto MID : RoadMeshMaterials)
		{
			if (IsValid(MID))
			{
				MID->SetScalarParameterValue(TEXT("CanBuild"), State);
			}
		}
	};

	if (Result == ERoadModuleTraceResult::NotColliding)
	{
		UpdateMaterials(1);
	}

	else if (Result == ERoadModuleTraceResult::BodyColliding)
	{
		UpdateMaterials(2);
	}

	else
	{
		UpdateMaterials(2);
	}
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