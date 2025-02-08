// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/LobbyPlayerPlatform.h"
#include "GameFramework/LobbyPlayerState.h"
#include "Vehicle/GearVehicle.h"
#include "Vehicle/GearDriver.h"
#include "Utils/DataHelperBFL.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"



ALobbyPlayerPlatform::ALobbyPlayerPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	RotationSocket = CreateDefaultSubobject<USceneComponent>(TEXT("RotationSocket"));
	RotationSocket->SetupAttachment(Root);

	DriverSocket = CreateDefaultSubobject<USceneComponent>(TEXT("DriverSocket"));
	DriverSocket->SetupAttachment(RotationSocket);

	Vehicle = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Vehicle"));
	Vehicle->SetupAttachment(RotationSocket);
	Vehicle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Vehicle->SetHiddenInGame(true);

	Hitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	Hitbox->SetupAttachment(Root);
	Hitbox->SetCollisionProfileName(TEXT("Selectable"));

	PlayerbarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerbarWidget"));
	PlayerbarWidget->SetupAttachment(Root);

	bGrabbed = false;

	RotationVelocity;
	RotationSpeed = 80.0f;
	RotationDrag = 4.0f;
	RotationDamping = 3.0f;
	RotationStopBias = 0.2f;

	PlatformIndex = 0;
}

void ALobbyPlayerPlatform::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (IsValid(Vehicle))
	{
		VehicleMID = Vehicle->CreateDynamicMaterialInstance(0);
	}

	if (IsValid(Hitbox))
	{
		Hitbox->OnClicked.AddDynamic(this, &ALobbyPlayerPlatform::OnHitboxClicked);
		Hitbox->OnReleased.AddDynamic(this, &ALobbyPlayerPlatform::OnHitboxReleased);
		Hitbox->OnInputTouchBegin.AddDynamic(this, &ALobbyPlayerPlatform::OnHitboxTouchBegin);
		Hitbox->OnInputTouchEnd.AddDynamic(this, &ALobbyPlayerPlatform::OnHitboxTouchEnd);
	}
}

void ALobbyPlayerPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALobbyPlayerPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FMath::Abs(RotationVelocity) > RotationStopBias)
	{
		float YawOffset = RotationVelocity * -RotationSpeed * DeltaTime;
		RotationSocket->SetWorldRotation(FRotator(0.0f, YawOffset + RotationSocket->GetComponentRotation().Yaw, 0.0f));

		RotationVelocity = FMath::FInterpTo(RotationVelocity, 0.0f, DeltaTime, RotationDrag);
		RotationVelocity = FMath::FInterpConstantTo(RotationVelocity, 0.0f, DeltaTime, RotationDamping);
	}
}

void ALobbyPlayerPlatform::Destroyed()
{
	ClearPlatform();
	
	Super::Destroyed();
}

void ALobbyPlayerPlatform::ClearPlatform()
{
	Vehicle->SetHiddenInGame(true);

	if (IsValid(Driver))
	{
		Driver->Destroy();
	}
}

class ALobbyPlayerState* ALobbyPlayerPlatform::GetOwningPlayer()
{
	return OwningPlayer;
}

void ALobbyPlayerPlatform::Rotate(float Amount)
{
	if (bGrabbed)
	{
		RotationVelocity += Amount;
	}
}

void ALobbyPlayerPlatform::ClearGrabbed()
{
	bGrabbed = false;
}

void ALobbyPlayerPlatform::StopRotation()
{
	RotationVelocity = 0.0f;
}

int32 ALobbyPlayerPlatform::GetPlatformIndex() const
{
	return PlatformIndex;
}

void ALobbyPlayerPlatform::OnHitboxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	StopRotation();
	bGrabbed = true;
}

void ALobbyPlayerPlatform::OnHitboxReleased(UPrimitiveComponent* TouchedComponent, FKey ButtonReleased)
{
	bGrabbed = false;
}

void ALobbyPlayerPlatform::OnHitboxTouchBegin(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	StopRotation();
	bGrabbed = true;
}

void ALobbyPlayerPlatform::OnHitboxTouchEnd(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	bGrabbed = false;
}

void ALobbyPlayerPlatform::SetOwningPlayer(class ALobbyPlayerState* InOwningPlayer)
{
	if (IsValid(OwningPlayer) && OwningPlayer == InOwningPlayer)
	{
		return;
	}

	if (IsValid(OwningPlayer))
	{
		OwningPlayer->OnPlayerCustomizationColorChanged.RemoveDynamic(this, &ALobbyPlayerPlatform::PlayerCustomizationColorChanged);
		OwningPlayer->OnPlayerCustomizationHeadChanged.RemoveDynamic(this, &ALobbyPlayerPlatform::PlayerCustomizationHeadChanged);
	}

	ClearPlatform();
	OwningPlayer = InOwningPlayer;

	if (IsValid(OwningPlayer))
	{
		Vehicle->SetHiddenInGame(false);
		OwningPlayer->OnPlayerCustomizationColorChanged.AddDynamic(this, &ALobbyPlayerPlatform::PlayerCustomizationColorChanged);
		OwningPlayer->OnPlayerCustomizationHeadChanged.AddDynamic(this, &ALobbyPlayerPlatform::PlayerCustomizationHeadChanged);

		Driver = GetWorld()->SpawnActor<AGearDriver>(DriverClass, DriverSocket->GetComponentTransform());
		if (IsValid(Driver))
		{
			Driver->AttachToComponent(DriverSocket, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

			PlayerCustomizationColorChanged();
			PlayerCustomizationHeadChanged();
		}
	}

	OnOwningPlayerChanged();
}

void ALobbyPlayerPlatform::PlayerCustomizationHeadChanged()
{
	if (IsValid(OwningPlayer))
	{
		if (IsValid(Driver))
		{
			Driver->ChangePlayerCustomizationHead(OwningPlayer->GetPlayerCustomization());
		}
	}
}

void ALobbyPlayerPlatform::PlayerCustomizationColorChanged()
{
	if (IsValid(OwningPlayer))
	{
		if (IsValid(VehicleMID))
		{
			VehicleMID->SetVectorParameterValue(TEXT("Color"), OwningPlayer->GetPlayerCustomization().TricycleColor.LinearColor);
		}

		if (IsValid(Driver))
		{
			Driver->ChangePlayerCustomizationColor(OwningPlayer->GetPlayerCustomization());
		}
	}
}