// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/VehicleCamera.h"
#include "Vehicle/GearVehicle.h"
#include "GameFramework/GearGameState.h"
#include "GameSystems/TrackSpline.h"
#include "Vehicle/VehicleSpringArm.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


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

void AVehicleCamera::BeginPlay()
{
	Super::BeginPlay();

	GearGameState = GetWorld()->GetGameState<AGearGameState>();
	UpdateCamera();
	MarkTeleport();
}

void AVehicleCamera::BecomeViewTarget(APlayerController* PC)
{
	Super::BecomeViewTarget(PC);

	OwnerController = PC;
	UpdateCameraMatrix();
}

void AVehicleCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCamera();
	UpdateCameraMatrix();
}

void AVehicleCamera::UpdateCamera()
{
#if WITH_EDITOR
	
	if (bTestMode)
	{
		UpdateCameraWithTransform(Vehicle->GetActorTransform());
		return;
	}

#endif

	if (IsValid(GearGameState))
	{
		FTransform TrackTransform = GearGameState->TrackSpline->GetTrackTransfsormAtDistance(GearGameState->FurthestReachedDistace);
		UpdateCameraWithTransform(TrackTransform);
	}
}

void AVehicleCamera::UpdateCameraMatrix()
{
	if (IsValid(OwnerController))
	{
		FMatrix ViewMatrix;
		FMatrix ProjectionMatrix;

		UGameplayStatics::CalculateViewProjectionMatricesFromViewTarget(this, ViewMatrix, ProjectionMatrix, ViewProjectionMatrix);

		int32 SizeX;
		int32 SizeY;
		OwnerController->GetViewportSize(SizeX, SizeY);
		ViewportSize = FVector2D(SizeX, SizeY);
	}
}

void AVehicleCamera::MarkTeleport()
{
	CameraBoom->TeleportToDesireLocation();
}

bool AVehicleCamera::IsOutsideCameraFrustum(AActor* Target)
{
	if (IsValid(OwnerController))
	{
		FVector2D ScreenPos;
		FSceneView::ProjectWorldToScreen(Target->GetActorLocation(), FIntRect(0, 0, ViewportSize.X, ViewportSize.Y), ViewProjectionMatrix, ScreenPos);

		return ScreenPos.X < 0 || ScreenPos.X > ViewportSize.X
			|| ScreenPos.Y < 0 || ScreenPos.Y > ViewportSize.Y;
	}

	return false;
}

void AVehicleCamera::UpdateCameraWithTransform(const FTransform& Transform)
{
	FVector Tangent = Transform.Rotator().Vector();
	Tangent.Z = 0;
	FRotator CameraRotation = UKismetMathLibrary::MakeRotFromXZ(Tangent, FVector::UpVector);

	SetActorLocation(Transform.GetLocation());
	SetActorRotation(CameraRotation);
}