// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrackSpline.generated.h"

class USplineComponent;
class AGearRoadModule;
struct FCrossTrackProperty;

UCLASS()
class GEAR_API ATrackSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	ATrackSpline();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USplineComponent* Spline;

public:	
	virtual void Tick(float DeltaTime) override;

	void RoadModuleStackChanged(const TArray<AGearRoadModule*> RoadModulesStack);

	void GetTrackPropertiesAtLocation(const FVector& Position, FCrossTrackProperty& CrossTrackProperty);

	float GetTrackDistanceAtPosition(const FVector& Position);
};
