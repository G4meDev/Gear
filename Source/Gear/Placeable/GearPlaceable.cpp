// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearPlaceable.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GearPlayerController.h"
#include "GameFramework/GearPlayerState.h"
#include "GameFramework/GearGameState.h"
#include "GameFramework/GearBuilderPawn.h"
#include "Placeable/PlaceableSpawnPoint.h"

AGearPlaceable::AGearPlaceable()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SelectionHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("SelectionHitbox"));
	SelectionHitbox->SetCollisionProfileName("Selectable");
	SelectionHitbox->SetupAttachment(Root);

	PreviewRotationPivot = CreateDefaultSubobject<USceneComponent>(TEXT("PreviewRotationPivot"));
	PreviewRotationPivot->SetupAttachment(Root);

	SelectionIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionIndicator"));
	SelectionIndicator->SetupAttachment(PreviewRotationPivot);
	SelectionIndicator->SetIsReplicated(true);

	PlaceableState = EPlaceableState::Idle;
	PreviewScale = 1.0f;

	bReplicates = true;
	SetReplicateMovement(true);
}

void AGearPlaceable::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SelectionHitbox->OnClicked.AddDynamic(this, &AGearPlaceable::OnSelectionBoxClicked);
	SelectionHitbox->OnInputTouchBegin.AddDynamic(this, &AGearPlaceable::OnSelectionBoxTouched);

	SelectionIndicatorMaterial = SelectionIndicator->CreateDynamicMaterialInstance(0, nullptr);

	SelectionIndicator->SetVisibility(false);
}

void AGearPlaceable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGearPlaceable, OwningPlayer);
	DOREPLIFETIME(AGearPlaceable, PlaceableState);
}

void AGearPlaceable::BeginPlay()
{
	Super::BeginPlay();
	
	if (PlaceableState == EPlaceableState::Preview)
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


}

void AGearPlaceable::RoundReset()
{
	RoundRestBlueprintEvent();
}

void AGearPlaceable::SetPreview()
{
	if (HasAuthority())
	{
		PlaceableState = EPlaceableState::Preview;
		SelectionIndicator->SetVisibility(false);
	}
}

void AGearPlaceable::SetSelectedBy(AGearBuilderPawn* Player)
{
	if (HasAuthority())
	{
		PlaceableState = EPlaceableState::Selected;
	}

	AGearPlayerState* PlayerState = Player->GetPlayerState<AGearPlayerState>();
	if (IsValid(PlayerState))
	{
		SelectionIndicatorMaterial->SetVectorParameterValue(TEXT("Color"), PlayerState->PlayerColor);
		SelectionIndicator->SetVisibility(true);
	}
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

void AGearPlaceable::AttachToSpawnPoint(APlaceableSpawnPoint* SpawnPoint)
{
	AttachToActor(SpawnPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SetActorScale3D(FVector(PreviewScale));
	SetActorRelativeLocation(-PreviewRotationPivot->GetRelativeLocation() * PreviewScale);
}