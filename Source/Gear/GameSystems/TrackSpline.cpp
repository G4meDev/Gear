// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/TrackSpline.h"
#include "Placeable/GearRoadModule.h"
#include "GameSystems/GearSplineComponent.h"

ATrackSpline::ATrackSpline()
{
	PrimaryActorTick.bCanEverTick = true;

	Spline = CreateDefaultSubobject<UGearSplineComponent>(TEXT("Spline"));
	SetRootComponent(Spline);

	bReplicates = true;
	bAlwaysRelevant = true;
}

void ATrackSpline::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATrackSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATrackSpline::RoadModuleStackChanged(const TArray<AGearRoadModule*> RoadModulesStack)
{
	Spline->ClearSplinePoints(false);

	check(!RoadModulesStack.IsEmpty());

	FSplinePoint SplinePoint;
	auto AddSplinePoint = [&](int StackIndex, int SplinePointIndex) 
	{
		SplinePoint = RoadModulesStack[StackIndex]->RoadSpline->GetSplinePointAt(SplinePointIndex, ESplineCoordinateSpace::World);

		SplinePoint.Type = RoadModulesStack[StackIndex]->RoadSpline->GetSplinePointType(SplinePointIndex);
		SplinePoint.Position = RoadModulesStack[StackIndex]->RoadSpline->GetLocationAtSplineInputKey(SplinePointIndex, ESplineCoordinateSpace::World);
		SplinePoint.ArriveTangent = RoadModulesStack[StackIndex]->RoadSpline->GetArriveTangentAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::World);
		SplinePoint.LeaveTangent = RoadModulesStack[StackIndex]->RoadSpline->GetLeaveTangentAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::World);
		SplinePoint.Rotation = RoadModulesStack[StackIndex]->RoadSpline->GetRotationAtSplineInputKey(SplinePointIndex, ESplineCoordinateSpace::World);
		SplinePoint.InputKey = Spline->GetNumberOfSplinePoints();

		Spline->AddPoint(SplinePoint, false);
	};

	AddSplinePoint(0, 0);

	for (int i = 0; i < RoadModulesStack.Num(); i++)
	{
		AGearRoadModule* RoadModule = RoadModulesStack[i];
		check(IsValid(RoadModule));

		for (int j = 1; j < RoadModule->RoadSpline->GetNumberOfSplinePoints(); j++)
		{
			AddSplinePoint(i, j);
		}
	}

	Spline->UpdateSpline();
}

void ATrackSpline::GetTrackPropertiesAtLocation(const FVector& Position, FCrossTrackProperty& CrossTrackProperty)
{
	float InputKey = Spline->FindInputKeyClosestToWorldLocation(Position);

	CrossTrackProperty.Position = Spline->GetLocationAtSplineInputKey(InputKey, ESplineCoordinateSpace::World);
	CrossTrackProperty.Tangent = Spline->GetTangentAtSplineInputKey(InputKey, ESplineCoordinateSpace::World);
	CrossTrackProperty.Up = Spline->GetUpVectorAtSplineInputKey(InputKey, ESplineCoordinateSpace::World);

	CrossTrackProperty.Right = FVector::CrossProduct(CrossTrackProperty.Up, CrossTrackProperty.Tangent).GetSafeNormal();
	CrossTrackProperty.FlattenRight = CrossTrackProperty.Right.GetSafeNormal2D();

	FVector NearestToPosition = Position - CrossTrackProperty.Position;
	CrossTrackProperty.Error = FVector::DotProduct(CrossTrackProperty.FlattenRight, NearestToPosition);
}

FTransform ATrackSpline::GetTrackTransfsormAtDistance(float Distance)
{
	return Spline->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
}

float ATrackSpline::GetTrackDistanceAtPosition(const FVector& Position)
{
	return Spline->GetDistanceAlongSplineAtLocation(Position, ESplineCoordinateSpace::World);
}
