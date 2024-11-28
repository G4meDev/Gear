// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/GearBuilderPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringarmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Targetpoint.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "InputAction.h"

AGearBuilderPawn::AGearBuilderPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	ViewTarget = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ViewTarget"));
	ViewTarget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(ViewTarget);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetupAttachment(ViewTarget);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	bCanMove = false;
	MovementSpeed = 100.0f;
}

void AGearBuilderPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(MoveScreenAction, ETriggerEvent::Triggered, this, &AGearBuilderPawn::MoveScreen);
}


void AGearBuilderPawn::BeginPlay()
{
	Super::BeginPlay();


	
}

void AGearBuilderPawn::MoveScreen(const FInputActionInstance& Instance)
{
	if (bCanMove)
	{
		FVector2D MovementInput = Instance.GetValue().Get<FVector2D>();
		FVector MovementInput3D = FVector(MovementInput.X, MovementInput.Y, 0);

		FVector DeltaLocation = GetTransform().InverseTransformVectorNoScale(MovementInput3D * MovementSpeed * GetWorld()->GetDeltaSeconds());
		SetActorLocation(GetActorLocation() + DeltaLocation);
	}
}

void AGearBuilderPawn::FindStartPlacingTarget(FVector& Location, FRotator& Rotation)
{
	TArray<AActor*> TargetActors;

	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ATargetPoint::StaticClass(), TEXT("Start"), TargetActors);

	if (TargetActors.Num() == 1)
	{
		Location = TargetActors[0]->GetActorLocation();
		Rotation = TargetActors[0]->GetActorRotation();
	}

	else
	{
		Location = FVector::Zero();
		Rotation = FRotator::ZeroRotator;
	}
}

void AGearBuilderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AGearBuilderPawn::StartPlacing()
{
	bCanMove = true;

	FVector TargetLocation;
	FRotator TargetRotation;

	FindStartPlacingTarget(TargetLocation, TargetRotation);
	SetActorLocationAndRotation(TargetLocation, TargetRotation);


}