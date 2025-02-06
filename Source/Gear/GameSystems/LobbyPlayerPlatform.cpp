// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/LobbyPlayerPlatform.h"
#include "GameFramework/LobbyPlayerState.h"
#include "Vehicle/GearVehicle.h"
#include "Vehicle/GearDriver.h"
#include "Utils/DataHelperBFL.h"



ALobbyPlayerPlatform::ALobbyPlayerPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	DriverSocket = CreateDefaultSubobject<USceneComponent>(TEXT("DriverSocket"));
	DriverSocket->SetupAttachment(Root);

	Vehicle = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Vehicle"));
	Vehicle->SetupAttachment(Root);
	Vehicle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Vehicle->SetHiddenInGame(true);
}

void ALobbyPlayerPlatform::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (IsValid(Vehicle))
	{
		VehicleMID = Vehicle->CreateDynamicMaterialInstance(0);
	}
}

void ALobbyPlayerPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALobbyPlayerPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void ALobbyPlayerPlatform::SetOwningPlayer(class ALobbyPlayerState* InOwningPlayer)
{
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