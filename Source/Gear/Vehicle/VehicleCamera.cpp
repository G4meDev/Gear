// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/VehicleCamera.h"
#include "GameFramework/SpringarmComponent.h"
#include "Camera/CameraComponent.h"

AVehicleCamera::AVehicleCamera()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetupAttachment(Root);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
}

void AVehicleCamera::MarkTeleport()
{
	CameraBoom->bEnableCameraLag = false;
	CameraBoom->bEnableCameraRotationLag = false;

	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AVehicleCamera::ClearTeleport));
}

void AVehicleCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

void AVehicleCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVehicleCamera::ClearTeleport()
{
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;
}
