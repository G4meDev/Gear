// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearPlaceable.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearGameState.h"

AGearPlaceable::AGearPlaceable()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SelectionHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("SelectionHitbox"));

	SelectionHitbox->SetupAttachment(Root);

	SelectionIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionIndicator"));
	SelectionIndicator->SetVisibility(false);
	SelectionIndicator->SetupAttachment(Root);
	SelectionIndicator->SetIsReplicated(true);

	PreviewRotaionOffset = FMath::FRandRange(0.0f, 360.0f);
	HazardState = EPlaceableState::Idle;


	bReplicates = true;
	SetReplicateMovement(true);
}

void AGearPlaceable::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SelectionHitbox->OnClicked.AddDynamic(this, &AGearPlaceable::OnSelectionBoxClicked);
	SelectionHitbox->OnInputTouchBegin.AddDynamic(this, &AGearPlaceable::OnSelectionBoxTouched);

	SelectionIndicatorMaterial = SelectionIndicator->CreateDynamicMaterialInstance(0, nullptr);
}

void AGearPlaceable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearPlaceable, OwningPlayer);
	DOREPLIFETIME(AGearPlaceable, HazardState);
}

void AGearPlaceable::BeginPlay()
{
	Super::BeginPlay();
	
	if (HazardState == EPlaceableState::Preview)
	{
		SetPreview();
	}
}

void AGearPlaceable::SelectionBoxClicked()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			AGearPlayerController* GearController = Cast<AGearPlayerController>(PlayerController);

			if (IsValid(GearController))
			{
				GearController->SelectPlaceable(this);
			}
		}
	}
}

void AGearPlaceable::OnSelectionBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	SelectionBoxClicked();
}

void AGearPlaceable::OnSelectionBoxTouched(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	SelectionBoxClicked();
}

void AGearPlaceable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// in preview mode rotate locally
	if (HazardState == EPlaceableState::Preview || HazardState == EPlaceableState::Selected)
	{
		FRotator Rotator = Rotator.ZeroRotator;
		Rotator.Yaw = GetWorld()->GetTimeSeconds() * PreviewRotationSpeed + PreviewRotaionOffset;

		SetActorRotation(Rotator);
	}
}

void AGearPlaceable::RoundReset()
{
	RoundRestBlueprintEvent();
}

void AGearPlaceable::SetPreview()
{
	if (HasAuthority())
	{
		HazardState = EPlaceableState::Preview;
		SelectionIndicator->SetVisibility(false);
	}
}

void AGearPlaceable::SetSelectedBy(AGearPlayerState* Player)
{
	if (HasAuthority())
	{
		HazardState = EPlaceableState::Selected;
	}

	SelectionIndicatorMaterial->SetVectorParameterValue(TEXT("Color"), Player->PlayerColor);
	SelectionIndicator->SetVisibility(true);
}

bool AGearPlaceable::HasOwningPlayer() const
{
	return OwningPlayer != nullptr;
}

void AGearPlaceable::OnRep_OwningPlayer()
{
	if (OwningPlayer == nullptr)
	{
		SetPreview();
	}
	else
	{
		SetSelectedBy(OwningPlayer);
	}
}