
// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/GearVehicle.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Vehicle/VehicleAudioComponent.h"

#include "GameFramework/GearPlayerController.h"
#include "Vehicle/GearDriver.h"
#include "Vehicle/WheelEffectType.h"
#include "UI/VehicleInputWidget.h"
#include "Ability/GearAbility.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearGameState.h"
#include "GameFramework/GearGameMode.h"
#include "GameFramework/GearHUD.h"
#include "Vehicle/VehicleCamera.h"
#include "GameSystems/TrackSpline.h"
#include "ChaosVehicleMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/UserWidget.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"

#define VEHICLE_TESTMAP_NAME "VehicleTestMap"


AGearVehicle::AGearVehicle()
{
	VehicleAudioComponent = CreateDefaultSubobject<UVehicleAudioComponent>(TEXT("VehicleAudioComponent"));
	VehicleAudioComponent->SetupAttachment(GetRootComponent());

	GetMesh()->bReceivesDecals = false;

	SteerValue				= 0.0f;
	SteerAngle				= 0.0f;
	ThrottleValue			= 0.0f;
	DistanaceAlongTrack		= 0.0f;
	BrakeValue				= 0.0f;
	TargetCheckpoint		= 1;
	bGrantedInvincibility	= false;

	AirTorqueControl		= 700.0f;

	WheelEffectOffset		= 0.0f;

	bReplicates				= true;
	bAlwaysRelevant			= true;
}

void AGearVehicle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearVehicle, bGrantedInvincibility);
	DOREPLIFETIME(AGearVehicle, TargetCheckpoint);
	DOREPLIFETIME(AGearVehicle, Ability);
	DOREPLIFETIME_CONDITION(AGearVehicle, SteerAngle, COND_SkipOwner);
}

void AGearVehicle::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(DriverClass))
	{
		Driver = GetWorld()->SpawnActor<AGearDriver>(DriverClass);
		Driver->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Driver->SetOwningVehicle(this);
	}

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

void AGearVehicle::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ReceiveControllerChangedDelegate.AddDynamic(this, &AGearVehicle::OnControllerChanged);
}

// server and owning client
void AGearVehicle::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	APlayerController* PC = GetController<APlayerController>();
	if (IsValid(PC) && PC->IsLocalController())
	{
// 		AGearHUD* HUD = PC->GetHUD<AGearHUD>();
// 		if (IsValid(HUD))
// 		{
// 			HUD->AddVehicleInputWidget();
// 		}

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

	APlayerController* PC = GetController<APlayerController>();
	if (IsValid(PC) && PC->IsLocalController())
	{
// 		AGearHUD* HUD = PC->GetHUD<AGearHUD>();
// 		if (IsValid(HUD))
// 		{
// 			HUD->RemoveVehicleInputWidget();
// 		}
	}

	if (IsValid(Driver))
	{
		Driver->Destroy();
	}

	if (IsValid(EliminationFXActorClass))
	{
		GetWorld()->SpawnActor<AActor>(EliminationFXActorClass, GetActorLocation(), GetActorRotation());
	}

	if (HasAuthority())
	{
		if (IsValid(Ability))
		{
			Ability->Destroy();
		}
	}

	Super::Destroyed();
}

void AGearVehicle::OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	if (IsValid(NewController) && NewController->IsLocalController())
	{
		APlayerController* PC = Cast<APlayerController>(NewController);
		AGearHUD* HUD = PC ? PC->GetHUD<AGearHUD>() : nullptr;
		if (IsValid(HUD))
		{
			HUD->AddVehicleInputWidget(this);
		}
	}

	else if (IsValid(OldController) && OldController->IsLocalController())
	{
		APlayerController* PC = Cast<APlayerController>(OldController);
		AGearHUD* HUD = PC ? PC->GetHUD<AGearHUD>() : nullptr;
		if (IsValid(HUD))
		{
			HUD->RemoveVehicleInputWidget();
		}
	}
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

		Input->BindAction(AbilityActionInput, ETriggerEvent::Triggered, this, &AGearVehicle::Input_Ability);
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

void AGearVehicle::Input_Ability(const FInputActionInstance& Instance)
{
	if (HasAbility())
	{
		Ability->ActivateAbility();
	}
}

void AGearVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentSpeed = GetVehicleMovement() ? FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) : 0.0f;

	UpdateWheelEffect(DeltaSeconds);
	
	if (IsLocallyControlled())
	{
		UpdateVehicleInputs();
	}

	if (HasAuthority() || IsLocallyControlled())
	{
		SteerAngle = GetWheelSteerAngle(0);
	}

	UpdateControl();

#if WITH_EDITOR
	if (bInTestMap)
		return;
#endif

	UpdateDistanceAlongTrack();
}

void AGearVehicle::UpdateVehicleInputs()
{
	GetVehicleMovementComponent()->SetSteeringInput(SteerValue);

	if (CanDrive())
	{
		GetVehicleMovementComponent()->SetThrottleInput(ThrottleValue);
		GetVehicleMovementComponent()->SetBrakeInput(BrakeValue);
	}
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

bool AGearVehicle::IsInvincible()
{
	return bGrantedInvincibility;
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
	TArray<AActor*> OutActors;
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(Bounds.SphereRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const bool bHasOccluders = GetWorld()->OverlapAnyTestByObjectType(Bounds.Origin, FQuat::Identity, FCollisionObjectQueryParams(ObjectTypeQuery), CollisionShape, QueryParams);
	//DrawDebugSphere(GetWorld(), Bounds.GetSphere().Center, Bounds.SphereRadius, 8, bHasOccluders ? FColor::Red : FColor::Blue, false, 0.1f);

	return !bHasOccluders;
}

bool AGearVehicle::IsSpectating()
{
	return IsValid(GetGearGameState()) ? GearGameState->FurthestReachedCheckpoint + 1 < TargetCheckpoint : true;
}

USkeletalMeshComponent* AGearVehicle::GetDriverBody()
{
	if(IsValid(Driver))
		return Driver->GetBodyMesh();

	return nullptr;
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
	return GetActorLocation().Z - TrackTransform.GetLocation().Z < -400.0f;
}

// ---------------------------------------------------------------------------

float AGearVehicle::GetWheelSteerAngle(int32 Index)
{
	auto& Wheels = GetChaosMovementComponent()->Wheels;
	return Index < Wheels.Num() && IsValid(Wheels[Index]) ? Wheels[Index]->GetSteerAngle() : 0.0f;
}

float AGearVehicle::GetWheelRotation(int32 Index)
{
	auto& Wheels = GetChaosMovementComponent()->Wheels;
	return Index < Wheels.Num() && IsValid(Wheels[Index]) ? Wheels[Index]->GetRotationAngle() : 0.0f;
}

float AGearVehicle::GetWheelRotationSpeed(int32 Index)
{
	auto& Wheels = GetChaosMovementComponent()->Wheels;
	return Index < Wheels.Num() && IsValid(Wheels[Index]) ? Wheels[Index]->GetRotationAngularVelocity() : 0.0f;
}

bool AGearVehicle::IsWheelOnGround(int32 Index)
{
	return IsValid(GetChaosMovementComponent()) ? GetChaosMovementComponent()->GetWheelState(Index).bInContact : false;
}

// const TWeakObjectPtr<UPhysicalMaterial> AGearVehicle::GetWheelContactPhysicMaterial(int32 Index)
// {
// 	if (IsValid(GetChaosMovementComponent()))
// 	{
// 		if (ChaosMovementComponent->GetWheelState(Index).bInContact)
// 		{
// 			return ChaosMovementComponent->GetWheelState(Index).PhysMaterial;
// 		}
// 	}
// 	
// 	return nullptr;
// }

bool AGearVehicle::IsWheelSkiding(int32 Index)
{
	return IsValid(GetChaosMovementComponent()) ? GetChaosMovementComponent()->GetWheelState(Index).bIsSkidding : false;
}

float AGearVehicle::GetSteerAngle()
{
	return SteerAngle;
}

bool AGearVehicle::HasAbility()
{
	return IsValid(Ability);
}

AGearAbility* AGearVehicle::GetAbility()
{
	return Ability;
}

void AGearVehicle::OnRep_Ability()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	AGearHUD* HUD = PC ? PC->GetHUD<AGearHUD>() : nullptr;
	if (IsValid(HUD))
	{
		HUD->AbilityStateChanged(Ability);
	}
}

void AGearVehicle::GrantAbility(TSubclassOf<class AGearAbility> AbilityClass)
{
	if (HasAuthority() && IsValid(AbilityClass))
	{
		Ability = GetWorld()->SpawnActor<AGearAbility>(AbilityClass);
		Ability->SetOwningVehicle(this);
		Ability->SetOwner(this);
		OnRep_Ability();
	}
}

void AGearVehicle::ClearAbility()
{
	if (HasAuthority())
	{
		Ability = nullptr;
		OnRep_Ability();
	}
}

AGearDriver* AGearVehicle::GetDriver()
{
	return Driver;
}

int32 AGearVehicle::GetNumWheelsOnGround()
{
	int Result = 0;

	if (IsValid(GetChaosMovementComponent()))
	{
		for (int i = 0; i < ChaosMovementComponent->GetNumWheels(); i++)
		{
			if (ChaosMovementComponent->GetWheelState(i).bInContact)
			{
				Result++;
			}
		}
	}

	return Result;
}

bool AGearVehicle::IsOnGround()
{
	return GetNumWheelsOnGround() == 0;
}

void AGearVehicle::ReduceVelocityBeRatio(float Ratio)
{
	if (IsValid(GetChaosMovementComponent()))
	{
		FVector Velocity = GetMesh()->GetComponentVelocity();		
		GetMesh()->AddImpulse(FMath::Clamp(Ratio, 0, 1) * -Velocity, NAME_None, true);
	}
}

void AGearVehicle::UpdateControl()
{
	const int32 NumWheelsOnGround = GetNumWheelsOnGround();
	
	InputDirection = FVector(ChaosMovementComponent->GetThrottleInput() - ChaosMovementComponent->GetBrakeInput()
		, ChaosMovementComponent->GetSteeringInput(), 0.0f);

	if (IsValid(GetChaosMovementComponent()))
	{
		if (NumWheelsOnGround == 0)
		{
			GetMesh()->AddTorqueInDegrees(GetActorUpVector() * InputDirection.Y * AirTorqueControl, NAME_None, true);
		}

	}
}

// ---------------------------------------------------------------------------------------------

void AGearVehicle::UpdateWheelEffect(float DeltaTime)
{
	bool bTiresTouchingGround = false;

	if (GetChaosMovementComponent() && IsValid(WheelEffect))
	{
		for (int32 i = 0; i < UE_ARRAY_COUNT(DustPSC); i++)
		{
			UPhysicalMaterial* ContactMat = ChaosMovementComponent->Wheels[i]->GetContactSurfaceMaterial();
			if (ContactMat != nullptr)
			{
				bTiresTouchingGround = true;
			}
			UNiagaraSystem* WheelFX = WheelEffect->GetFX(ContactMat, CurrentSpeed);
			
			const bool bIsActive = DustPSC[i] != nullptr && DustPSC[i]->IsActive() && !DustPSC[i]->IsComplete();
			UNiagaraSystem* CurrentFX = DustPSC[i] != nullptr ? Cast<UNiagaraSystem>(DustPSC[i]->GetFXSystemAsset()) : nullptr;
			
			if (WheelFX != nullptr && (CurrentFX != WheelFX || !bIsActive))
			{
				if (DustPSC[i] == nullptr || DustPSC[i]->IsActive())
				{
					if (DustPSC[i] != nullptr)
					{
						DustPSC[i]->Deactivate();
						DustPSC[i]->SetAutoDestroy(true);
					}
					SpawnNewWheelEffect(i);
				}
				DustPSC[i]->SetAsset(WheelFX);
				DustPSC[i]->ActivateSystem();
			}
			else if (WheelFX == nullptr && bIsActive)
			{
				DustPSC[i]->Deactivate();
				DustPSC[i]->SetAutoDestroy(true);
				DustPSC[i] = nullptr;
			}
		}
	}
}

void AGearVehicle::SpawnNewWheelEffect(int WheelIndex)
{
	DustPSC[WheelIndex] = NewObject<UNiagaraComponent>(this);
	DustPSC[WheelIndex]->bAutoActivate = true;
	DustPSC[WheelIndex]->SetAutoDestroy(false);
	DustPSC[WheelIndex]->RegisterComponentWithWorld(GetWorld());
	DustPSC[WheelIndex]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	FVector WheelLocalPosition = GetMesh()->GetBoneLocation(ChaosMovementComponent->WheelSetups[WheelIndex].BoneName, EBoneSpaces::ComponentSpace);
	DustPSC[WheelIndex]->SetRelativeLocation(WheelLocalPosition + FVector::DownVector * (ChaosMovementComponent->Wheels[WheelIndex]->WheelRadius - WheelEffectOffset));
}