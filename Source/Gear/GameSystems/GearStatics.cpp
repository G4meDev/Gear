// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/GearStatics.h"
#include "Placeable/GearRoadModule.h"
#include "Vehicle/GearVehicle.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"

ERoadModuleTraceResult UGearStatics::TraceRoadModule(UObject* WorldContextObject, TSubclassOf<class AGearRoadModule> RoadModuleClass, const FTransform& SocketTransform)
{
	AGearRoadModule* RoadModule = RoadModuleClass.GetDefaultObject();

	FVector ColliderPos;
	FQuat ColliderRot;
	FCollisionShape TraceShape;

	auto TraceForCollider = [&](UBoxComponent* Collider)
	{
		ColliderPos = SocketTransform.TransformPosition(Collider->GetRelativeLocation());
		ColliderRot = SocketTransform.TransformRotation(Collider->GetRelativeRotation().Quaternion());
		TraceShape = FCollisionShape::MakeBox(Collider->GetUnscaledBoxExtent());

		return WorldContextObject->GetWorld()->OverlapAnyTestByProfile(ColliderPos, ColliderRot, TEXT("RoadBoundCollider"), TraceShape);
	};

	bool bHitMainBody = TraceForCollider(RoadModule->MainCollider);

#if !UE_BUILD_SHIPPING
	//DrawDebugBox(WorldContextObject->GetWorld(), ColliderPos, RoadModule->MainCollider->GetUnscaledBoxExtent(), ColliderRot, bHitMainBody ? FColor::Red : FColor::Blue, false, 0.1f);
#endif

	if (bHitMainBody)
		return ERoadModuleTraceResult::BodyColliding;


	bool bHitExtent = TraceForCollider(RoadModule->ExtentCollider);

#if !UE_BUILD_SHIPPING
	//DrawDebugBox(WorldContextObject->GetWorld(), ColliderPos, RoadModule->MainCollider->GetUnscaledBoxExtent(), ColliderRot, bHitExtent ? FColor::Red : FColor::Yellow, false, 0.1f);
#endif

	if (bHitExtent)
		return ERoadModuleTraceResult::ExtentColliding;

	return ERoadModuleTraceResult::NotColliding;
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