// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/GearBuilderPawn.h"
#include "GameFramework/GearGameState.h"
#include "GameFramework/GearPlayerState.h"
#include "Placeable/GearPlaceable.h"
#include "Placeable/PlaceableSocket.h"
#include "Placeable/GearRoadModule.h"
#include "Placeable/GearHazard.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringarmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Targetpoint.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "InputAction.h"
#include "Net/UnrealNetwork.h"

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
	MovementSpeed = 80.0f;
	Damping = 3.0f;
	Drag = 4.0f;
	ScreenDragValue = FVector2D::Zero();
	Velocity = FVector2D::Zero();

	SelectedSocket = nullptr;

	BuilderPawnState = EBuilderPawnState::Preview;
	bSelectedMirroredX = false;
	bSelectedMirroredY = false;
	bPlacingUnhandled = false;
	

	bReplicates = true;
}

void AGearBuilderPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearBuilderPawn, SelectedPlaceable);
	DOREPLIFETIME(AGearBuilderPawn, SelectedPlaceableClass);
}

void AGearBuilderPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(MoveScreenAction, ETriggerEvent::Triggered, this, &AGearBuilderPawn::MoveScreenInputTrigger);
	Input->BindAction(MoveScreenAction, ETriggerEvent::Completed, this, &AGearBuilderPawn::MoveScreenInputCompleted);
}

void AGearBuilderPawn::MoveScreenInputTrigger(const FInputActionInstance& Instance)
{
	ScreenDragValue = Instance.GetValue().Get<FVector2D>();
}


void AGearBuilderPawn::MoveScreenInputCompleted(const FInputActionInstance& Instance)
{
	ScreenDragValue = FVector2D::Zero();
}

void AGearBuilderPawn::BeginPlay()
{
	Super::BeginPlay();

}

void AGearBuilderPawn::Destroyed()
{
	Super::Destroyed();
	
}

void AGearBuilderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanMove)
	{
		Velocity += -ScreenDragValue * MovementSpeed * DeltaTime;

		if (Velocity.IsNearlyZero(KINDA_SMALL_NUMBER))
		{
			Velocity = FVector2D::Zero();
		}

		else
		{
			float VelocityMagnitude = Velocity.Length();
			Velocity.Normalize();
			
			VelocityMagnitude = FMath::FInterpTo(VelocityMagnitude, 0.0f, DeltaTime, Drag);
			VelocityMagnitude = FMath::FInterpConstantTo(VelocityMagnitude, 0.0f, DeltaTime, Damping);

			Velocity *= VelocityMagnitude;

			FVector Velocity3D = FVector(Velocity.X, Velocity.Y, 0);
			FVector DeltaLocation = GetTransform().InverseTransformVectorNoScale(Velocity3D);

			SetActorLocation(GetActorLocation() + DeltaLocation);
		}
	}
}

void AGearBuilderPawn::OnRep_SelectedPlaceable(AGearPlaceable* OldSelected)
{

}

void AGearBuilderPawn::OnRep_SelectedPlaceableClass()
{
	if (bPlacingUnhandled)
	{
		AGearGameState* GearGameState = Cast<AGearGameState>(UGameplayStatics::GetGameState(GetWorld()));
		if (IsValid(GearGameState) && GearGameState->GearMatchState == EGearMatchState::Placing)
		{
			StartPlacing();
		}
	}
}

void AGearBuilderPawn::StartPlacing()
{
	if (IsLocallyControlled())
	{
		if (!IsValid(SelectedPlaceableClass))
		{
			bPlacingUnhandled = true;
			return;
		}

		bCanMove = true;
		TeleportToRoadEnd();

		if (SelectedPlaceableClass->IsChildOf<AGearRoadModule>())
		{
			BuilderPawnState = EBuilderPawnState::PlacingRoadModules;
			SpawnPlacingRoadModules();
			UpdatePlacingRoadModule(bSelectedMirroredX, bSelectedMirroredY);
		}

		else if(SelectedPlaceableClass->IsChildOf<AGearHazard>())
		{
			BuilderPawnState = EBuilderPawnState::PlacingHazards;
		}
		

	}

	//UpdateSelectedPlaceable();
}

void AGearBuilderPawn::SpawnPlacingRoadModules()
{
	FTransform SpawnTransform = FTransform::Identity;
	AActor* SpawnedActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), SelectedPlaceableClass, SpawnTransform);

	if (IsValid(SpawnedActor))
	{
		PlacingRoadModule = Cast<AGearRoadModule>(SpawnedActor);
		if (IsValid(PlacingRoadModule))
		{
			PlacingRoadModule->MarkNotReplicated();
			UGameplayStatics::FinishSpawningActor(PlacingRoadModule, SpawnTransform);
		}
		else
		{
			SpawnedActor->Destroy();
		}
	}

	if (IsValid(PlacingRoadModule) && IsValid(PlacingRoadModule->RoadModuleMirroredClass))
	{
		SpawnedActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), PlacingRoadModule->RoadModuleMirroredClass, SpawnTransform);

		if (IsValid(SpawnedActor))
		{
			PlacingRoadModuleMirroredY = Cast<AGearRoadModule>(SpawnedActor);
			if (IsValid(PlacingRoadModuleMirroredY))
			{
				PlacingRoadModuleMirroredY->MarkNotReplicated();
				UGameplayStatics::FinishSpawningActor(PlacingRoadModuleMirroredY, SpawnTransform);
			}
			else
			{
				SpawnedActor->Destroy();
			}
		}
	}
}

void AGearBuilderPawn::UpdatePlacingRoadModule(bool bMirroredX, bool bMirroredY)
{
	AGearGameState* GearGameState = Cast<AGearGameState>(UGameplayStatics::GetGameState(GetWorld()));

	if (IsValid(GearGameState))
	{
		UPlaceableSocket* RoadEndSocket = GearGameState->GetRoadEndSocket();

		if (IsValid(RoadEndSocket))
		{
			bSelectedMirroredX = bMirroredX;
			bSelectedMirroredY = bMirroredY;

			AGearRoadModule* ActiveRoadModule = bMirroredY ? PlacingRoadModuleMirroredY : PlacingRoadModule;
			AGearRoadModule* DeactiveRoadModule = !bMirroredY ? PlacingRoadModuleMirroredY : PlacingRoadModule;

			if (IsValid(ActiveRoadModule))
			{
				FVector TargetLocation = RoadEndSocket->GetComponentLocation();
				FRotator TargetRotation = RoadEndSocket->GetComponentRotation();

				if (bSelectedMirroredX)
				{
					FRotator DeltaRotator = FRotator(0, 180, 0) - ActiveRoadModule->RoadEndSocket->GetRelativeRotation();
					TargetRotation = RoadEndSocket->GetComponentTransform().TransformRotation(DeltaRotator.Quaternion()).Rotator();

					FVector DeltaLocation = DeltaRotator.RotateVector(ActiveRoadModule->RoadEndSocket->GetRelativeLocation());
					TargetLocation = RoadEndSocket->GetComponentTransform().TransformPositionNoScale(-DeltaLocation);
				}

				ActiveRoadModule->SetActorLocationAndRotation(TargetLocation, TargetRotation);
			}

			if (IsValid(DeactiveRoadModule))
			{
				DeactiveRoadModule->SetActorLocationAndRotation(FVector::Zero(), FRotator::ZeroRotator);
			}

			SelectedSocket = RoadEndSocket;
		}
	}
}

void AGearBuilderPawn::TeleportToRoadEnd()
{
	AGearGameState* GearGameState = Cast<AGearGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (IsValid(GearGameState))
	{
		UPlaceableSocket* RoadEndSocket = GearGameState->GetRoadEndSocket();
		if (IsValid(RoadEndSocket))
		{
			Velocity = FVector2D::Zero();
			SetActorLocationAndRotation(RoadEndSocket->GetComponentLocation(), RoadEndSocket->GetComponentRotation());
		}
	}
}

void AGearBuilderPawn::SetSelectedPlaceable(AGearPlaceable* Placeable)
{
	if (HasAuthority())
	{
		if (IsValid(SelectedPlaceable))
		{
			SelectedPlaceable->OwningPlayer = nullptr;
			SelectedPlaceable->OnRep_OwningPlayer();
		}

		if (IsValid(Placeable))
		{
			Placeable->OwningPlayer = this;
			Placeable->OnRep_OwningPlayer();
		}

		SelectedPlaceable = Placeable;
	}
}

bool AGearBuilderPawn::HasSelectedPlaceable() const
{
	return SelectedPlaceable != nullptr;
}