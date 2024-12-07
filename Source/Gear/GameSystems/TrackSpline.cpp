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

	FSplinePoint SplinePoint = RoadModulesStack[0]->RoadSpline->GetSplinePointAt(0, ESplineCoordinateSpace::World);
	SplinePoint.Position += RoadModulesStack[0]->GetActorLocation();
	SplinePoint.InputKey = 0;
	Spline->AddPoint(SplinePoint, false);

	for (int i = 1; i < RoadModulesStack.Num(); i++)
	{
		AGearRoadModule* RoadModule = RoadModulesStack[i];
		check(IsValid(RoadModule));

		for (int j = 0; j < RoadModule->RoadSpline->GetNumberOfSplinePoints(); j++)
		{
			SplinePoint = RoadModule->RoadSpline->GetSplinePointAt(j, ESplineCoordinateSpace::World);
			SplinePoint.Position += RoadModule->GetActorLocation();
			SplinePoint.InputKey = Spline->GetNumberOfSplinePoints();
			Spline->AddPoint(SplinePoint, false);
		}
	}

	Spline->UpdateSpline();
}
