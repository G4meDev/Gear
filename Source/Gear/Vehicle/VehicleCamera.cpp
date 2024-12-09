// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/VehicleCamera.h"
#include "GameFramework/GearGameState.h"
#include "GameSystems/TrackSpline.h"
#include "Vehicle/VehicleSpringArm.h"
#include "Camera/CameraComponent.h"


AVehicleCamera::AVehicleCamera()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = ETickingGroup::TG_PostPhysics;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CameraBoom = CreateDefaultSubobject<UVehicleSpringArm>(TEXT("CameraBoom"));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetupAttachment(Root);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
}

void AVehicleCamera::UpdateCamera()
{
	if (IsValid(GearGameState))
	{
		FVector WorldPos = GearGameState->TrackSpline->GetTrackLocationAtDistance(GearGameState->FurthestReachedDistace);
		SetActorLocation(WorldPos);
	}
}

void AVehicleCamera::BeginPlay()
{
	Super::BeginPlay();
	
	GearGameState = GetWorld()->GetGameState<AGearGameState>();
 	UpdateCamera();
	CameraBoom->TeleportToDesireLocation();
}

void AVehicleCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	

	UpdateCamera();
}