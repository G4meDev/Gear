
// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/GearVehicle.h"
#include "ChaosWheeledVehicleMovementComponent.h"

#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearGameState.h"
#include "GameFramework/GearGameMode.h"
#include "Vehicle/VehicleCamera.h"
#include "GameSystems/TrackSpline.h"
#include "ChaosVehicleMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"

#define VEHICLE_TESTMAP_NAME "VehicleTestMap"


AGearVehicle::AGearVehicle()
{
	SteerValue = 0.0f;
	ThrottleValue = 0.0f;
	BrakeValue = 0.0f;

	bAlwaysRelevant = true;
	DistanaceAlongTrack = 0;
	TargetCheckpoint = 1;

	bGrantedInvincibility = false;
}

void AGearVehicle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearVehicle, bGrantedInvincibility);
	DOREPLIFETIME(AGearVehicle, TargetCheckpoint);
}

void AGearVehicle::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	
	bInTestMap = GetWorld()->GetName().Equals(VEHICLE_TESTMAP_NAME);

	if (bInTestMap)
	{
		AController* CC = GetController();
		if (IsValid(CC))
		{
			NotifyControllerChanged();
		}

		return;
	}

#endif

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AGearPlayerController* PC = Cast<AGearPlayerController>(*It);
		if (IsValid(PC) && PC->IsLocalController())
		{
			if (HasAuthority())
			{
				AuthorativeController = PC;
			}

			break;
		}
	}

}

// only server
void AGearVehicle::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}

// server and owning client
void AGearVehicle::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	APlayerController* PC = GetController<APlayerController>();
	if (IsValid(PC) && PC->IsLocalController())
	{
		VehicleInputWidget = CreateWidget(GetWorld(), VehicleInputWidgetClass);
		VehicleInputWidget->AddToViewport();

#if WITH_EDITOR

		if (bInTestMap)
		{
			FTransform SpawnTransform;
			AActor* SpawnActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), VehicleCameraClass, SpawnTransform);

			AVehicleCamera* VehicleCamera = Cast<AVehicleCamera>(SpawnActor);
			VehicleCamera->bTestMode = true;
			VehicleCamera->Vehicle = this;
			UGameplayStatics::FinishSpawningActor(VehicleCamera, SpawnTransform);

			PC->SetViewTarget(VehicleCamera);
		}
#endif
	}
}

void AGearVehicle::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);

#if WITH_EDITOR
	if (bInTestMap)
		return;
#endif

	AGearPlayerState* GearPlayerState = GetPlayerState<AGearPlayerState>();
	if (IsValid(GearPlayerState) && IsValid(GetVehicleMaterial()))
	{
		VehicleMaterial->SetVectorParameterValue(TEXT("Color"), GearPlayerState->PlayerColor.ReinterpretAsLinear());
	}
}

void AGearVehicle::Destroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Killed"), *GetName());

#if WITH_EDITOR
	if (bInTestMap)
		return;
#endif

	if (IsValid(VehicleInputWidget))
	{
		VehicleInputWidget->RemoveFromParent();
		VehicleInputWidget = nullptr;
	}

	Super::Destroyed();
}

void AGearVehicle::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (IsLocallyControlled())
	{
		UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
		Input->BindAction(SteerActionInput, ETriggerEvent::Triggered, this, &AGearVehicle::Input_Steer);
		Input->BindAction(SteerActionInput, ETriggerEvent::Completed, this, &AGearVehicle::Input_Steer);

		Input->BindAction(ThrottleActionInput, ETriggerEvent::Triggered, this, &AGearVehicle::Input_Throttle);
		Input->BindAction(ThrottleActionInput, ETriggerEvent::Completed, this, &AGearVehicle::Input_Throttle);

		Input->BindAction(BrakeActionInput, ETriggerEvent::Triggered, this, &AGearVehicle::Input_Brake);
		Input->BindAction(BrakeActionInput, ETriggerEvent::Completed, this, &AGearVehicle::Input_Brake);
	}
}

AGearGameState* AGearVehicle::GetGearGameState()
{
	if (IsValid(GearGameState))
	{
		return GearGameState;
	}

	GearGameState = GetWorld()->GetGameState<AGearGameState>();
	return GearGameState;
}

void AGearVehicle::Input_Steer(const FInputActionInstance& Instance)
{
	SteerValue = Instance.GetValue().Get<float>();
}

void AGearVehicle::Input_Throttle(const FInputActionInstance& Instance)
{
	ThrottleValue = Instance.GetValue().Get<float>();
}

void AGearVehicle::Input_Brake(const FInputActionInstance& Instance)
{
	BrakeValue = Instance.GetValue().Get<float>();
}

void AGearVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (IsLocallyControlled())
	{
		GetVehicleMovementComponent()->SetSteeringInput(SteerValue);

		if (CanDrive())
		{
			GetVehicleMovementComponent()->SetThrottleInput(ThrottleValue);
			GetVehicleMovementComponent()->SetBrakeInput(BrakeValue);
		}
	}

#if WITH_EDITOR
	if (bInTestMap)
		return;
#endif

	UpdateDistanceAlongTrack();
}

bool AGearVehicle::CanDrive()
{
#if WITH_EDITOR
	if (bInTestMap) return true;
#endif

	bool bRaceStarted = IsValid(GetGearGameState()) ? !GearGameState->IsCountDown() : false;
	return bRaceStarted && !IsSpectating();
}

void AGearVehicle::UpdateDistanceAlongTrack()
{
	DistanaceAlongTrack = GetGearGameState()->TrackSpline->GetTrackDistanceAtPosition(GetActorLocation());
}

void AGearVehicle::UpdateStateToVehicle(AGearVehicle* TargetVehicle)
{
	if (IsValid(TargetVehicle) && IsValid(TargetVehicle->GetChaosMovementComponent()) && IsValid(GetChaosMovementComponent()))
	{
		FWheeledSnaphotData SnapshotData = TargetVehicle->GetChaosMovementComponent()->GetSnapshot();
		SnapshotData.Transform = GetActorTransform();
		for (FWheelSnapshot WheelSnapshot : SnapshotData.WheelSnapshots)
		{
			WheelSnapshot.SteeringAngle = 0.0f;
		}

		ChaosMovementComponent->SetSnapshot(SnapshotData);
	}
}

UChaosWheeledVehicleMovementComponent* AGearVehicle::GetChaosMovementComponent()
{
	if (IsValid(ChaosMovementComponent))
	{
		return ChaosMovementComponent;
	}

	ChaosMovementComponent = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent());
	return ChaosMovementComponent;
}

UMaterialInstanceDynamic* AGearVehicle::GetVehicleMaterial()
{
	if (IsValid(VehicleMaterial))
	{
		return VehicleMaterial;
	}

	VehicleMaterial = GetMesh()->CreateDynamicMaterialInstance(0, VehicleMaterialParent_Opaque);
	return VehicleMaterial;
}

void AGearVehicle::GrantInvincibility()
{
	if (HasAuthority())
	{
		bGrantedInvincibility = true;
		InvincibilityStartTime = GetWorld()->GetTimeSeconds();
	}
}

void AGearVehicle::RemoveInvincibility()
{
	if (HasAuthority())
	{
		bGrantedInvincibility = false;
	}
}

bool AGearVehicle::HasInvincibility() const
{
	return bGrantedInvincibility;
}

bool AGearVehicle::CanRemoveInvincibility()
{
	FBoxSphereBounds3d Bounds = GetMesh()->Bounds;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery;
	ObjectTypeQuery.Add(UEngineTypes::ConvertToObjectType(ECC_Vehicle));
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	TArray<AActor*> OutActors;

	const bool bHasOccluders = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Bounds.GetSphere().Center, Bounds.SphereRadius, ObjectTypeQuery, nullptr, ActorsToIgnore, OutActors);
	//DrawDebugSphere(GetWorld(), Bounds.GetSphere().Center, Bounds.SphereRadius, 8, bHasOccluders ? FColor::Red : FColor::Blue, false, 0.1f);

	return !bHasOccluders;
}

bool AGearVehicle::IsSpectating()
{
	return IsValid(GetGearGameState()) ? GearGameState->FurthestReachedCheckpoint + 1 != TargetCheckpoint : true;
}

void AGearVehicle::OnRep_GrantedInvincibility()
{
	if (HasInvincibility())
	{
		VehicleMaterial = GetMesh()->CreateDynamicMaterialInstance(0, VehicleMaterialParent_Transparent);
		GetMesh()->SetCollisionProfileName(TEXT("InvincibleVehicle"));
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("removed invincibility from %s"), *GetName());

		VehicleMaterial = GetMesh()->CreateDynamicMaterialInstance(0, VehicleMaterialParent_Opaque);
		GetMesh()->SetCollisionProfileName(TEXT("Vehicle"));
	}
}

bool AGearVehicle::IsOutsideTrack() const
{
	FTransform TrackTransform = GearGameState->TrackSpline->GetTrackTransfsormAtDistance(DistanaceAlongTrack);
	return GetActorLocation().Z - TrackTransform.GetLocation().Z < -700.0f;
}