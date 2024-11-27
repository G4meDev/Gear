// Fill out your copyright notice in the Description page of Project Settings.


#include "Hazard/GearHazardActor.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearGameState.h"

AGearHazardActor::AGearHazardActor()
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

void AGearHazardActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SelectionHitbox->OnClicked.AddDynamic(this, &AGearHazardActor::OnSelectionBoxClicked);
	SelectionHitbox->OnInputTouchBegin.AddDynamic(this, &AGearHazardActor::OnSelectionBoxTouched);

	SelectionIndicatorMaterial = SelectionIndicator->CreateDynamicMaterialInstance(0, nullptr);	
}

void AGearHazardActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearHazardActor, HazardState);
	DOREPLIFETIME(AGearHazardActor, OwningPlayer);
}

void AGearHazardActor::BeginPlay()
{
	Super::BeginPlay();

	if (HazardState == EHazardState::Preview)
	{
		SetPreview();
	}
}

void AGearHazardActor::SelectionBoxClicked()
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

void AGearHazardActor::OnSelectionBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	SelectionBoxClicked();
}

void AGearHazardActor::OnSelectionBoxTouched(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	SelectionBoxClicked();
}

void AGearHazardActor::SetPreview()
{
	if (HasAuthority())
	{
		HazardState = EHazardState::Preview;
		SelectionIndicator->SetVisibility(false);
	}
}

void AGearHazardActor::SetSelectedBy(AGearPlayerState* Player)
{
	if (HasAuthority())
	{
		HazardState = EHazardState::Selected;
	}

	SelectionIndicatorMaterial->SetVectorParameterValue(TEXT("Color"), Player->PlayerColor);
	SelectionIndicator->SetVisibility(true);
}

void AGearHazardActor::OnRep_OwningPlayer()
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

bool AGearHazardActor::HasOwningPlayer() const
{
	return OwningPlayer != nullptr;
}

void AGearHazardActor::Tick(float DeltaTime)
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