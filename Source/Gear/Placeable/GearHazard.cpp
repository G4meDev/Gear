// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearHazard.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearGameState.h"

AGearHazard::AGearHazard()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SelectionHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("SelectionHitbox"));

	SelectionHitbox->SetupAttachment(Root);

	SelectionIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionIndicator"));
	SelectionIndicator->SetVisibility(false);
	SelectionIndicator->SetupAttachment(Root);
	SelectionIndicator->SetIsReplicated(true);

	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	PreviewRotaionOffset = FMath::FRandRange(0.0f, 360.0f);
	HazardState = EHazardState::Idle;
}

void AGearHazard::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SelectionHitbox->OnClicked.AddDynamic(this, &AGearHazard::OnSelectionBoxClicked);
	SelectionHitbox->OnInputTouchBegin.AddDynamic(this, &AGearHazard::OnSelectionBoxTouched);

	SelectionIndicatorMaterial = SelectionIndicator->CreateDynamicMaterialInstance(0, nullptr);	
}

void AGearHazard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearHazard, HazardState);
	DOREPLIFETIME(AGearHazard, OwningPlayer);
}

void AGearHazard::BeginPlay()
{
	Super::BeginPlay();

	if (HazardState == EHazardState::Preview)
	{
		SetPreview();
	}
}

void AGearHazard::SelectionBoxClicked()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			AGearPlayerController* GearController = Cast<AGearPlayerController>(PlayerController);

			if (IsValid(GearController))
			{
				GearController->SelectHazard(this);
			}
		}
	}
}

void AGearHazard::OnSelectionBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	SelectionBoxClicked();
}

void AGearHazard::OnSelectionBoxTouched(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	SelectionBoxClicked();
}

void AGearHazard::SetPreview()
{
	if (HasAuthority())
	{
		HazardState = EHazardState::Preview;
		SelectionIndicator->SetVisibility(false);
	}
}

void AGearHazard::SetSelectedBy(AGearPlayerState* Player)
{
	if (HasAuthority())
	{
		HazardState = EHazardState::Selected;
	}

	SelectionIndicatorMaterial->SetVectorParameterValue(TEXT("Color"), Player->PlayerColor);
	SelectionIndicator->SetVisibility(true);
}

void AGearHazard::OnRep_OwningPlayer()
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

bool AGearHazard::HasOwningPlayer() const
{
	return OwningPlayer != nullptr;
}

void AGearHazard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// in preview mode rotate locally
	if (HazardState == EHazardState::Preview || HazardState == EHazardState::Selected)
	{
		FRotator Rotator = Rotator.ZeroRotator;
		Rotator.Yaw = GetWorld()->GetTimeSeconds() * PreviewRotationSpeed + PreviewRotaionOffset;

		SetActorRotation(Rotator);
	}

}