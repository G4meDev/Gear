// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/GearBuilderPawn.h"
#include "GameFramework/GearplayerController.h"
#include "GameFramework/GearGameState.h"
#include "GameFramework/GearPlayerState.h"
#include "Placeable/GearPlaceable.h"
#include "Placeable/GearRoadModule.h"
#include "Placeable/GearHazard.h"
#include "Placeable/HazardSocketMarker.h"
#include "Placeable/HazardSocketComponent.h"
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

	RemainingHazardCount = 0;

	bAlwaysRelevant = true;
	bReplicates = true;
}

void AGearBuilderPawn::UnPossessed()
{
	if (HasAuthority() && IsValid(this))
	{
		Destroy();
	}
}

void AGearBuilderPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearBuilderPawn, SelectedPlaceable);
	DOREPLIFETIME(AGearBuilderPawn, SelectedPlaceableClass);
	DOREPLIFETIME(AGearBuilderPawn, RemainingHazardCount);
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
			BuilderPawnState = EBuilderPawnState::Idle;
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
		UpdateHazardMarkers();
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

			RoadModule->InitializePrebuildMaterials();
			RoadModule->SetMainColliderEnabled(false);
			RoadModule->PrebuildModules->SetHiddenInGame(false, true);
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

	PlacingRoadModule->OnTraceStateChanged(ERoadModuleTraceResult::NotColliding);
	PlacingRoadModule_MirroredX->OnTraceStateChanged(ERoadModuleTraceResult::NotColliding);
	PlacingRoadModule_MirroredY->OnTraceStateChanged(ERoadModuleTraceResult::NotColliding);
	PlacingRoadModule_MirroredX_MirroredY->OnTraceStateChanged(ERoadModuleTraceResult::NotColliding);
}

AGearRoadModule* AGearBuilderPawn::GetActiveRoadModule()
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
		
		AGearRoadModule* ActiveRoadModule = GetActiveRoadModule();

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

			RoadModule->SetActorHiddenInGame(RoadModule == ActiveRoadModule ? false : true);
		};

		MoveRoadModule(PlacingRoadModule);
		MoveRoadModule(PlacingRoadModule_MirroredX);
		MoveRoadModule(PlacingRoadModule_MirroredY);
		MoveRoadModule(PlacingRoadModule_MirroredX_MirroredY);
	}
}

// gets called on begin play of road module and occupied event of hazard socket
void AGearBuilderPawn::UpdateHazardMarkers()
{
	if (BuilderPawnState == EBuilderPawnState::PlacingHazards)
	{
		DestroyHazardMarkers();

		TSubclassOf<AHazardSocketMarker> SocketMarkerClass = SelectedPlaceableClass->GetDefaultObject<AGearHazard>()->SocketMarkerClass;

		if (IsValid(SocketMarkerClass))
		{
			TArray<AActor*> RoadModules;
			UGameplayStatics::GetAllActorsOfClass(this, AGearRoadModule::StaticClass(), RoadModules);

			for (AActor* RoadModuleActor : RoadModules)
			{
				AGearRoadModule* RoadModule = Cast<AGearRoadModule>(RoadModuleActor);
				if (IsValid(RoadModule))
				{
					TInlineComponentArray<UHazardSocketComponent*> HazardSockets;
					RoadModule->GetComponents<UHazardSocketComponent>(HazardSockets);

					for (UHazardSocketComponent* HazardSocket : HazardSockets)
					{
						if (IsValid(HazardSocket) && !HazardSocket->IsOccupied())
						{
							FTransform SpawnTransform = HazardSocket->GetComponentTransform();
							AHazardSocketMarker* HazardSocketMarker = GetWorld()->SpawnActor<AHazardSocketMarker>(SocketMarkerClass, SpawnTransform.GetLocation(), SpawnTransform.Rotator());
							HazardSocketMarker->TargetSocket = HazardSocket;
						}
					}
				}
			}
		}
	}
}

void AGearBuilderPawn::DestroyHazardMarkers()
{
	TArray<AActor*> HazardMarkers;
	UGameplayStatics::GetAllActorsOfClass(this, AHazardSocketMarker::StaticClass(), HazardMarkers);

	for (AActor* Actor : HazardMarkers)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
}

void AGearBuilderPawn::Cleanup_SpawnedActors()
{
	auto CleanActor = [&](AActor* Actor)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
			Actor = nullptr;
		}
	};

	CleanActor(PlacingRoadModule);
	CleanActor(PlacingRoadModule_MirroredX);
	CleanActor(PlacingRoadModule_MirroredY);
	CleanActor(PlacingRoadModule_MirroredX_MirroredY);

	DestroyHazardMarkers();
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