// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/GearStatics.h"
#include "Placeable/GearRoadModule.h"
#include "Vehicle/GearVehicle.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"

bool UGearStatics::TracePlacingRoadModuleForCollision(UObject* WorldContextObject, TSubclassOf<class AGearRoadModule> RoadModuleClass, const FTransform& SocketTransform)
{
	AGearRoadModule* RoadModule = RoadModuleClass.GetDefaultObject();

	auto TraceBox = [&](const FCollisionBox& Box)
		{
			FVector ColliderPos;
			FQuat ColliderRot;
			FCollisionShape TraceShape;

			ColliderPos = SocketTransform.TransformPosition(Box.Location);
			ColliderRot = SocketTransform.TransformRotation(Box.Rotation.Quaternion());
			TraceShape = FCollisionShape::MakeBox(Box.Extent);

			bool bHit = WorldContextObject->GetWorld()->OverlapAnyTestByProfile(ColliderPos, ColliderRot, TEXT("RoadBoundCollider"), TraceShape);

#if !UE_BUILD_SHIPPING
			DrawDebugBox(WorldContextObject->GetWorld(), ColliderPos, Box.Extent, ColliderRot, bHit ? FColor::Red : FColor::Blue, false, 0.1f);
#endif
	
			return bHit;
		};

	if (IsValid(RoadModule))
	{
		for (const FCollisionBox& Box : RoadModule->PlacingColliderBoxs)
		{
			if (TraceBox(Box))
			{
				return true;
			}
		}
	}

	return false;
}

void UGearStatics::SphereOverlapForVehicles(UObject* WorldContextObject, TArray<class AGearVehicle*>& Vehicles, const FVector& Center, float Radius, TArray<AActor*> IgnoreActors, bool bIncludeInvincible)
{
	TArray<FOverlapResult> OverlapResults;
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_Vehicle);
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(Radius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActors(IgnoreActors);

	WorldContextObject->GetWorld()->OverlapMultiByObjectType(OverlapResults, Center, FQuat::Identity, QueryParams, SphereShape, Params);

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		AGearVehicle* GearVehicle = Cast<AGearVehicle>(OverlapResult.GetActor());
		if (IsValid(GearVehicle))
		{
			if (bIncludeInvincible || !GearVehicle->IsInvincible())
			{
				Vehicles.Add(GearVehicle);
			}
		}
	}
}