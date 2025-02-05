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
		OwningPlayer->OnPlayerCustomizationChanged.RemoveDynamic(this, &ALobbyPlayerPlatform::PlayerCustomizationChanged);
	}

	ClearPlatform();
	OwningPlayer = InOwningPlayer;

	if (IsValid(OwningPlayer))
	{
		Vehicle->SetHiddenInGame(false);
		OwningPlayer->OnPlayerCustomizationChanged.AddDynamic(this, &ALobbyPlayerPlatform::PlayerCustomizationChanged);

		Driver = GetWorld()->SpawnActor<AGearDriver>(DriverClass);
		if (IsValid(Driver))
		{
			Driver->AttachToComponent(DriverSocket, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Driver->ChangeDriverHead(UDataHelperBFL::GetHeadClassFromType(OwningPlayer->GetPlayerCustomization().HeadType));
		}
	}
}

void ALobbyPlayerPlatform::PlayerCustomizationChanged()
{
	if (IsValid(OwningPlayer))
	{
		if (IsValid(VehicleMID))
		{
			VehicleMID->SetVectorParameterValue(TEXT("Color"), UDataHelperBFL::ResolveColorCode(OwningPlayer->GetPlayerCustomization().TricycleColor).ReinterpretAsLinear());
		}
	}
}