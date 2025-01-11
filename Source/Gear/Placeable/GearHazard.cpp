// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearHazard.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"


AGearHazard::AGearHazard()
{
	PrimaryActorTick.bCanEverTick = true;


}

void AGearHazard::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

// void AGearHazard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
// 
// 
// 	
// }

#if WITH_EDITOR

void AGearHazard::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName ProperyName = PropertyChangedEvent.Property == nullptr ? NAME_None : PropertyChangedEvent.Property->GetFName();

	if (ProperyName == GET_MEMBER_NAME_CHECKED(AGearHazard, bDirty))
	{
		bDirty = false;

// 		FVector MinBound;
// 		FVector MaxBound;
// 		GetHazardBounds(MinBound, MaxBound);
// 		FVector BoundExtent = MaxBound - MinBound;
// 		FVector BoundCenter = (MinBound + MaxBound) / 2.0f;
// 
// 		FVector RotationPivotLocation = BoundCenter + MinBound.Z + RotationPivotHeightOffset;
// 		PreviewRotationPivot->SetRelativeLocation(RotationPivotLocation);
// 
// 		SelectionHitbox->SetRelativeLocation(BoundCenter);
// 		SelectionHitbox->SetBoxExtent(BoundExtent / 2 + SelectionHitboxPadding);
	}
}

void AGearHazard::GetHazardBounds_Implementation(FVector& Min, FVector& Max)
{
	Min = FVector(0.0f);
	Max = FVector(600.0f);
}

#endif

void AGearHazard::BeginPlay()
{
	Super::BeginPlay();


}

void AGearHazard::SetPreview()
{
	Super::SetPreview();
}

void AGearHazard::SetSelectedBy(AGearBuilderPawn* Player)
{
	Super::SetSelectedBy(Player);
}

void AGearHazard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}