// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearRoadModule.h"
#include "GameFramework/GearGameState.h"
#include "GameSystems/GearStatics.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/GearBuilderPawn.h"
#include "GameFramework/GearPlayerController.h"
#include "Net/UnrealNetwork.h"

#define DEFINE_COLLIDER( name )		name = CreateDefaultSubobject<UBoxComponent>(TEXT(#name));	\
 	name->SetupAttachment(MainColliders);														\
 	name->SetCollisionEnabled(ECollisionEnabled::NoCollision);									\
 	name->ShapeColor = FColor::Blue;

AGearRoadModule::AGearRoadModule()
{
	PrimaryActorTick.bCanEverTick = true;

	RoadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoadMesh"));
	RoadMesh->SetupAttachment(MainModules);
	RoadMesh->SetCollisionProfileName(TEXT("BlockAll"));

	RoadEndSocketComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RoadEndSocket"));
	RoadEndSocketComponent->SetupAttachment(Root);

	HazardSockets = CreateDefaultSubobject<USceneComponent>(TEXT("HazardSockets"));
	HazardSockets->SetupAttachment(ModulesStack);

	SpawnableSockets = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnableSockets"));
	SpawnableSockets->SetupAttachment(ModulesStack);

	RoadSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RoadSpline"));
	RoadSpline->SetupAttachment(Root);

	MainColliders = CreateDefaultSubobject<USceneComponent>(TEXT("MainColliders"));
	MainColliders->SetupAttachment(Root);

	DEFINE_COLLIDER(ExtentCollider);
	DEFINE_COLLIDER(Collider_1);
	DEFINE_COLLIDER(Collider_2);
	DEFINE_COLLIDER(Collider_3);
	DEFINE_COLLIDER(Collider_4);
	DEFINE_COLLIDER(Collider_5);
	DEFINE_COLLIDER(Collider_6);
	DEFINE_COLLIDER(Collider_7);
	DEFINE_COLLIDER(Collider_8);

	PreviewScale = 0.3f;

	bPlacingModuleInCollision = false;
	bActivePlacingModule = false;

	bShouldPlayBuildAnim = false;
}

void AGearRoadModule::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AGearRoadModule, bShouldPlayBuildAnim, COND_InitialOnly);
}

void AGearRoadModule::PostInitializeComponents()
{
	Super::PostInitializeComponents();


}

void AGearRoadModule::BeginPlay()
{
	Super::BeginPlay();

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (IsValid(PlayerController) && PlayerController->IsLocalController())
		{
			AGearBuilderPawn* BuilderPawn = PlayerController->GetPawn<AGearBuilderPawn>();
			if (IsValid(BuilderPawn))
			{
				BuilderPawn->UpdateHazardMarkers();
			}
		}
	}

	OnTraceStateChanged();

	if (bShouldPlayBuildAnim)
	{
		PlayBuildAnim();
	}
}

#if WITH_EDITOR

void AGearRoadModule::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName ProperyName = PropertyChangedEvent.Property == nullptr ? NAME_None : PropertyChangedEvent.Property->GetFName();

	if (ProperyName == GET_MEMBER_NAME_CHECKED(AGearRoadModule, bDirty))
	{
		bDirty = false;

		UpdateSplineParameters();
		UpdateColliders();
	}

	else if (ProperyName == GET_MEMBER_NAME_CHECKED(AGearRoadModule, bSplineDirty))
	{
		bSplineDirty = false;
		UpdateSplineFromParent(bMirrorX, bMirrorY);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void AGearRoadModule::UpdateSplineParameters()
{
	RoadLength = RoadSpline->GetSplineLength();

	if (IsValid(RoadMesh) && IsValid(PreviewRotationPivot))
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
	if (IsValid(RoadMesh))
	{
		FVector MinBound;
		FVector MaxBound;
		RoadMesh->GetLocalBounds(MinBound, MaxBound);

		FVector BoundOrigin = (MinBound + MaxBound) / 2;
		BoundOrigin = ModulesStack->GetRelativeTransform().TransformPosition(BoundOrigin);

		FVector BoundExtent = (MaxBound - MinBound) / 2;
		CollidersRefreneTransform = FTransform(FRotator::ZeroRotator, BoundOrigin, BoundExtent + MainColliderPadding);

		SelectionHitbox->SetRelativeLocation(BoundOrigin);
		SelectionHitbox->SetBoxExtent(BoundExtent + SelectionBoxPadding);

		BoundOrigin = RoadEndSocketComponent->GetRelativeTransform().TransformPosition(FVector(ExtentColliderSize.X, 0, 0));
		BoundExtent = ExtentColliderSize;

		ExtendCollidersRefreneTransform = FTransform(RoadEndSocketComponent->GetRelativeRotation(), BoundOrigin, BoundExtent);
	
// ------------------------------------------------------------------------------------------------------------------------------

		auto AddBox = [&](UBoxComponent* Box)
			{
				if (IsValid(Box) && Box->bDynamicObstacle)
				{
					FVector Origin = Box->GetRelativeLocation();
					FRotator Rotation = Box->GetRelativeRotation();
					FVector Extent = Box->GetUnscaledBoxExtent();

					PlacingColliderBoxs.Add(FCollisionBox(Origin, Rotation, Extent));
				}
			}; 

		PlacingColliderBoxs.Empty();

		AddBox(ExtentCollider);
		AddBox(Collider_1);
		AddBox(Collider_2);
		AddBox(Collider_3);
		AddBox(Collider_4);
		AddBox(Collider_5);
		AddBox(Collider_6);
		AddBox(Collider_7);
		AddBox(Collider_8);
	}

}

void AGearRoadModule::UpdateSplineFromParent(bool InMirrorX, bool InMirrorY)
{
	AGearRoadModule* RoadModuleCDO = IsValid(RoadModuleClass) ? RoadModuleClass->GetDefaultObject<AGearRoadModule>() : nullptr;
	if (IsValid(RoadModuleCDO) && IsValid(RoadModuleCDO->RoadSpline) && IsValid(RoadSpline))
	{
		FTransform EndSocketTransform = RoadEndSocketComponent->GetRelativeTransform();

		FSplinePoint SplinePoint;
		auto AddSplinePoint = [&](int SplinePointIndex)
			{
				SplinePoint = RoadModuleCDO->RoadSpline->GetSplinePointAt(SplinePointIndex, ESplineCoordinateSpace::World);

				SplinePoint.Type = RoadModuleCDO->RoadSpline->GetSplinePointType(SplinePointIndex);
				SplinePoint.Position = RoadModuleCDO->RoadSpline->GetLocationAtSplineInputKey(SplinePointIndex, ESplineCoordinateSpace::World);
				SplinePoint.ArriveTangent = RoadModuleCDO->RoadSpline->GetArriveTangentAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::World);
				SplinePoint.LeaveTangent = RoadModuleCDO->RoadSpline->GetLeaveTangentAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::World);
				SplinePoint.Rotation = RoadModuleCDO->RoadSpline->GetRotationAtSplineInputKey(SplinePointIndex, ESplineCoordinateSpace::World);
				SplinePoint.InputKey = RoadSpline->GetNumberOfSplinePoints();

				if (bMirrorY)
				{
					SplinePoint.Position = SplinePoint.Position * FVector(1, -1, 1);
					SplinePoint.ArriveTangent = SplinePoint.ArriveTangent * FVector(1, -1, 1);
					SplinePoint.LeaveTangent = SplinePoint.LeaveTangent * FVector(1, -1, 1);
				}

				if (bMirrorX)
				{
					FTransform Transform = EndSocketTransform;
					Transform.SetRotation(FQuat(FVector::UpVector, PI) * Transform.GetRotation());

					SplinePoint.Position = Transform.TransformPosition(SplinePoint.Position);

					Transform.SetRotation(FRotator(0, -EndSocketTransform.Rotator().Yaw, 0).Quaternion());

					SplinePoint.ArriveTangent = Transform.InverseTransformVectorNoScale(SplinePoint.ArriveTangent * FVector(1, 1, -1));
					SplinePoint.LeaveTangent = Transform.InverseTransformVectorNoScale(SplinePoint.LeaveTangent * FVector(1, 1, -1));
				}

				RoadSpline->AddPoint(SplinePoint, false);
			};

		RoadSpline->ClearSplinePoints(false);

		if (InMirrorX)
		{
			for (int i = RoadModuleCDO->RoadSpline->GetNumberOfSplinePoints(); i >= 0; i--)
			{
				AddSplinePoint(i);
			}
		}

		else
		{
			for (int i = 0; i < RoadModuleCDO->RoadSpline->GetNumberOfSplinePoints(); i++)
			{
				AddSplinePoint(i);
			}
		}

		RoadSpline->UpdateSpline();
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

	if (bActivePlacingModule)
	{
		bool NewResult = UGearStatics::TracePlacingRoadModuleForCollision(this, GetClass(), GetActorTransform());
		if (NewResult != bPlacingModuleInCollision)
		{
			bPlacingModuleInCollision = NewResult;
			OnTraceStateChanged();
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

USceneComponent* AGearRoadModule::GetAttachableSocket()
{
	return RoadEndSocketComponent;
}

void AGearRoadModule::SetPrebuildState(EPrebuildState State)
{
	uint8 BuildState = static_cast<uint8>(State);

	for (auto* MID : PrebuildMaterials)
	{
		if (IsValid(MID))
		{
			MID->SetScalarParameterValue(TEXT("State"), BuildState);
		}
	}
}

void AGearRoadModule::OnTraceStateChanged()
{
	SetPrebuildState(bPlacingModuleInCollision ? EPrebuildState::NotPlaceable : EPrebuildState::Placable);
}

void AGearRoadModule::InitializePrebuildMaterials()
{
	TArray<USceneComponent*> PrebuildChildComponents;
	PrebuildModules->GetChildrenComponents(true, PrebuildChildComponents);

	for (USceneComponent* Child : PrebuildChildComponents)
	{
		UStaticMeshComponent* ChildStaticMesh = IsValid(Child) ? Cast<UStaticMeshComponent>(Child) : nullptr;
		if (IsValid(ChildStaticMesh))
		{
			for (int32 i = 0; i < ChildStaticMesh->GetNumMaterials(); i++)
			{
				UMaterialInstanceDynamic* MID = ChildStaticMesh->CreateDynamicMaterialInstance(i, PrebuildMaterial);
				check(MID);

				PrebuildMaterials.Add(MID);
			}
		}
	}
}

// -----------------------------------------------------------------------------

void AGearRoadModule::SetMainColliderEnabled(bool bEnabled)
{
	auto SetBoxColliderEnabled = [&](UBoxComponent* Box)
		{
			if (IsValid(Box) && Box->bDynamicObstacle)
			{
				if (bEnabled)
				{
					Box->SetCollisionProfileName(TEXT("RoadBoundCollider"));
				}
				else
				{
					Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				}
			}
		};

	SetBoxColliderEnabled(Collider_1);
	SetBoxColliderEnabled(Collider_2);
	SetBoxColliderEnabled(Collider_3);
	SetBoxColliderEnabled(Collider_4);
	SetBoxColliderEnabled(Collider_5);
	SetBoxColliderEnabled(Collider_6);
	SetBoxColliderEnabled(Collider_7);
	SetBoxColliderEnabled(Collider_8);
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

void AGearRoadModule::OnEnabled_Start()
{
	Super::OnEnabled_Start();

	RoadMesh->SetHiddenInGame(false);
}

void AGearRoadModule::OnEnabled_End()
{
	Super::OnEnabled_End();

	RoadMesh->SetHiddenInGame(true);
}