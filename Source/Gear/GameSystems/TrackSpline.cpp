// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/TrackSpline.h"
#include "Components/SplineComponent.h"
#include "Placeable/GearRoadModule.h"

ATrackSpline::ATrackSpline()
{
	PrimaryActorTick.bCanEverTick = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(Spline);
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
	auto AddSplinePoint = [&](int StackIndex, int SplinePointIndex, bool bMirrored) 
	{
		SplinePoint = RoadModulesStack[StackIndex]->RoadSpline->GetSplinePointAt(SplinePointIndex, ESplineCoordinateSpace::World);
		SplinePoint.Position = RoadModulesStack[StackIndex]->RoadSpline->GetLocationAtSplineInputKey(SplinePointIndex, ESplineCoordinateSpace::World);
		SplinePoint.ArriveTangent = RoadModulesStack[StackIndex]->RoadSpline->GetArriveTangentAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::World);
		SplinePoint.LeaveTangent = RoadModulesStack[StackIndex]->RoadSpline->GetLeaveTangentAtSplinePoint(SplinePointIndex, ESplineCoordinateSpace::World);
		SplinePoint.Rotation = RoadModulesStack[StackIndex]->RoadSpline->GetRotationAtSplineInputKey(SplinePointIndex, ESplineCoordinateSpace::World);
		SplinePoint.InputKey = Spline->GetNumberOfSplinePoints();

		if (bMirrored)
		{
			FVector UpVector = RoadModulesStack[StackIndex]->RoadSpline->GetUpVectorAtSplineInputKey(SplinePointIndex, ESplineCoordinateSpace::World);

			SplinePoint.ArriveTangent = SplinePoint.ArriveTangent.RotateAngleAxis(180.0f, UpVector);
			SplinePoint.LeaveTangent = SplinePoint.LeaveTangent.RotateAngleAxis(180.0f, UpVector);
			SplinePoint.Rotation = SplinePoint.Rotation + FRotator(0, 180.0f, 0);
		}

		Spline->AddPoint(SplinePoint, false);
	};

	AddSplinePoint(0, 0, false);

	for (int i = 0; i < RoadModulesStack.Num(); i++)
	{
		AGearRoadModule* RoadModule = RoadModulesStack[i];
		check(IsValid(RoadModule));

		if (RoadModule->bMirrorX)
		{
			for (int j = RoadModule->RoadSpline->GetNumberOfSplinePoints() - 2; j >= 0 ; j--)
			{
				AddSplinePoint(i, j, true);
			}
		}
		else
		{
			for (int j = 1; j < RoadModule->RoadSpline->GetNumberOfSplinePoints(); j++)
			{
				AddSplinePoint(i, j, false);
			}
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
