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

	bPlacingUnhandled = false;

	bReplicates = true;
}

void AGearBuilderPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearBuilderPawn, SelectedPlaceable);
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
	if (!HasSelectedPlaceable())
	{
		bPlacingUnhandled = true;
		return;
	}

	bCanMove = true;
	TeleportToRoadEnd();

	SelectedPlaceable->SetActorScale3D(FVector::One());
	UpdateSelectedPlaceable();
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

void AGearBuilderPawn::FlipSelectedRoadModule()
{
	AGearRoadModule* RoadModule = Cast<AGearRoadModule>(SelectedPlaceable);
	if (IsValid(RoadModule))
	{
		RoadModule->Flip();
	}
}

void AGearBuilderPawn::UpdateSelectedPlaceable()
{
	if (IsValid(SelectedPlaceable))
	{
		if (SelectedPlaceable->IsA(AGearRoadModule::StaticClass()))
		{
			MoveSelectedRoadModuleToEnd();
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

void AGearBuilderPawn::MoveSelectedRoadModuleToEnd()
{
	AGearGameState* GearGameState = Cast<AGearGameState>(UGameplayStatics::GetGameState(GetWorld()));

	if (IsValid(GearGameState) && IsValid(SelectedPlaceable))
	{
		UPlaceableSocket* RoadEndSocket = GearGameState->GetRoadEndSocket();

		if (IsValid(RoadEndSocket))
		{
			SelectedPlaceable->SetActorLocationAndRotation(RoadEndSocket->GetComponentLocation(), RoadEndSocket->GetComponentRotation());
			SelectedSocket = RoadEndSocket;
		}
	}
}