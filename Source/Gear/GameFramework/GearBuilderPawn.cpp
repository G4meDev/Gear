// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/GearBuilderPawn.h"
#include "GameFramework/GearplayerController.h"
#include "GameFramework/GearGameState.h"
#include "GameFramework/GearPlayerState.h"
#include "Placeable/GearPlaceable.h"
#include "Placeable/PlaceableSocket.h"
#include "Placeable/GearRoadModule.h"
#include "Placeable/GearHazard.h"
#include "GameSystems/GearStatics.h"

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

	BuilderPawnState = EBuilderPawnState::Preview;
	bSelectedMirroredX = false;
	bSelectedMirroredY = false;
	
	bPlacedModule = false;

	bReplicates = true;
}

void AGearBuilderPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearBuilderPawn, SelectedPlaceable);
	DOREPLIFETIME(AGearBuilderPawn, SelectedPlaceableClass);
	DOREPLIFETIME(AGearBuilderPawn, bPlacedModule);
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

void AGearBuilderPawn::NotifyControllerChanged()
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (IsValid(PlayerController) && PlayerController->IsLocalController())
	{
		PlayerController->SetViewTarget(this);
	}
}

void AGearBuilderPawn::Destroyed()
{
	Super::Destroyed();
	
	Cleanup_SpawnedActors();
}

void AGearBuilderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AGearRoadModule* ActiveRoadModule = GetActiveRoadModule();

	if (ActiveRoadModule)
	{
		UGearStatics::TraceRoadModule(this, ActiveRoadModule->GetClass(), GetActorTransform());
	}

	if (IsLocallyControlled() && bCanMove)
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
	if (IsLocallyControlled())
	{
		if (IsValid(SelectedPlaceableClass))
		{
			StartPlacing();
		}

		else if (SelectedPlaceableClass == nullptr)
		{
			Cleanup_SpawnedActors();
		}
	}
}

void AGearBuilderPawn::OnRoadModuleSocketChanged()
{
	if (BuilderPawnState == EBuilderPawnState::PlacingRoadModules)
	{
		UpdatePlacingRoadModule(bSelectedMirroredX, bSelectedMirroredY);
	}
}

void AGearBuilderPawn::StartPlacing()
{
	bCanMove = true;
	TeleportToRoadEnd();

	if (SelectedPlaceableClass->IsChildOf<AGearRoadModule>())
	{
		BuilderPawnState = EBuilderPawnState::PlacingRoadModules;
		SpawnPlacingRoadModules();
		UpdatePlacingRoadModule(bSelectedMirroredX, bSelectedMirroredY);
	}

	else if (SelectedPlaceableClass->IsChildOf<AGearHazard>())
	{
		BuilderPawnState = EBuilderPawnState::PlacingHazards;
	}
		


}

AGearRoadModule* AGearBuilderPawn::SpawnRoadModuleLocally(TSubclassOf<AGearPlaceable> SpawnClass)
{
	FTransform SpawnTransform = FTransform::Identity;
	AActor* SpawnedActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), SpawnClass, SpawnTransform);
	AGearRoadModule* RoadModule = Cast<AGearRoadModule>(SpawnedActor);

	if (IsValid(SpawnedActor))
	{
		if (IsValid(RoadModule))
		{
			RoadModule->MarkNotReplicated();
			UGameplayStatics::FinishSpawningActor(RoadModule, SpawnTransform);
		}
		else
		{
			SpawnedActor->Destroy();
		}
	}

	return RoadModule;
}

void AGearBuilderPawn::SpawnPlacingRoadModules()
{
	PlacingRoadModule						= SpawnRoadModuleLocally(SelectedPlaceableClass);
	PlacingRoadModule_MirroredX				= SpawnRoadModuleLocally(PlacingRoadModule->RoadModuleClass_MirrorX->GetAuthoritativeClass());
	PlacingRoadModule_MirroredY				= SpawnRoadModuleLocally(PlacingRoadModule->RoadModuleClass_MirrorY->GetAuthoritativeClass());
	PlacingRoadModule_MirroredX_MirroredY	= SpawnRoadModuleLocally(PlacingRoadModule->RoadModuleClass_MirrorX_MirrorY->GetAuthoritativeClass());
}

AGearRoadModule* AGearBuilderPawn::GetActiveRoadModule() const
{
	if (!bSelectedMirroredX && !bSelectedMirroredY)
		return PlacingRoadModule;

	else if (bSelectedMirroredX && !bSelectedMirroredY)
		return PlacingRoadModule_MirroredX;

	else if (!bSelectedMirroredX && bSelectedMirroredY)
		return PlacingRoadModule_MirroredY;

	return PlacingRoadModule_MirroredX_MirroredY;
}

void AGearBuilderPawn::UpdatePlacingRoadModule(bool bMirroredX, bool bMirroredY)
{
	AGearGameState* GearGameState = Cast<AGearGameState>(UGameplayStatics::GetGameState(GetWorld()));

	if (IsValid(GearGameState))
	{
		bSelectedMirroredX = bMirroredX;
		bSelectedMirroredY = bMirroredY;
		
		const AGearRoadModule* ActiveRoadModule = GetActiveRoadModule();

		auto MoveRoadModule = [&](AGearRoadModule* RoadModule)
		{
			if (RoadModule == ActiveRoadModule)
			{
				const FTransform& RoadModuleSocket = GearGameState->RoadModuleSocketTransform;
				RoadModule->MoveToSocketTransform(RoadModuleSocket);
			}
			else
			{
				RoadModule->SetActorLocationAndRotation(FVector::Zero(), FRotator::ZeroRotator);
			}
		};

		MoveRoadModule(PlacingRoadModule);
		MoveRoadModule(PlacingRoadModule_MirroredX);
		MoveRoadModule(PlacingRoadModule_MirroredY);
		MoveRoadModule(PlacingRoadModule_MirroredX_MirroredY);
	}
}

void AGearBuilderPawn::Cleanup_SpawnedActors()
{
	auto CleanRoadModule = [&](AGearRoadModule * RoadModule)
	{
		if (IsValid(RoadModule))
		{
			RoadModule->Destroy();
			RoadModule = nullptr;
		}
	};

	CleanRoadModule(PlacingRoadModule);
	CleanRoadModule(PlacingRoadModule_MirroredX);
	CleanRoadModule(PlacingRoadModule_MirroredY);
	CleanRoadModule(PlacingRoadModule_MirroredX_MirroredY);
}

void AGearBuilderPawn::TeleportToRoadEnd()
{
	AGearGameState* GearGameState = Cast<AGearGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (IsValid(GearGameState))
	{		
		Velocity = FVector2D::Zero();
		SetActorLocation(GearGameState->RoadModuleSocketTransform.GetLocation());
	}
}

void AGearBuilderPawn::PlaceRoadModule()
{
	if (IsLocallyControlled())
	{
		const AGearRoadModule* ActiveRoadModule = GetActiveRoadModule();

		if (IsValid(ActiveRoadModule))
		{
			AGearGameState* GearGameState = GetWorld()->GetGameState<AGearGameState>();
			AGearPlayerController* GearController = GetController<AGearPlayerController>();
			if (IsValid(GearController) && IsValid(GearGameState))
			{
				GearController->PlaceRoadModule(ActiveRoadModule->GetClass(), bSelectedMirroredX);
			}
		}
	}
}

void AGearBuilderPawn::SetSelectedPlaceable(AGearPlaceable* Placeable)
{
	if (HasAuthority())
	{
		if (IsValid(SelectedPlaceable))
		{
			SelectedPlaceable->SetSelectedBy(nullptr);
		}

		if (IsValid(Placeable))
		{
			Placeable->SetSelectedBy(this);
		}

		SelectedPlaceable = Placeable;
	}
}

bool AGearBuilderPawn::HasSelectedPlaceable() const
{
	return SelectedPlaceable != nullptr;
}