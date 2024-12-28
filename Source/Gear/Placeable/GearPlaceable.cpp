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
	SelectionHitbox->SetupAttachment(Root);

	PreviewRotationPivot = CreateDefaultSubobject<USceneComponent>(TEXT("PreviewRotationPivot"));
	PreviewRotationPivot->SetupAttachment(Root);

	SelectionIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionIndicator"));
	SelectionIndicator->SetupAttachment(PreviewRotationPivot);
	SelectionIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PlaceableState = EPlaceableState::None;
	PreviewScale = 1.0f;

	bReplicates = true;
	SetReplicateMovement(true);
	bAlwaysRelevant = true;
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
	

}

void AGearPlaceable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AGearPlaceable::SelectionBoxClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Selected %s"), *GetName());

	if (PlaceableState == EPlaceableState::Preview)
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
}

void AGearPlaceable::OnSelectionBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	SelectionBoxClicked();
}

void AGearPlaceable::OnSelectionBoxTouched(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	SelectionBoxClicked();
}

void AGearPlaceable::MarkNotReplicated()
{
	SetReplicates(false);
	SetReplicateMovement(false);
}

void AGearPlaceable::RoundReset()
{
	RoundRestBlueprintEvent();
}

void AGearPlaceable::SetPreview()
{
	if (HasAuthority())
	{
		EPlaceableState OldState = PlaceableState;

		PlaceableState = EPlaceableState::Preview;

		OnRep_PlaceableState(OldState);
	}
}

void AGearPlaceable::SetSelectedBy(AGearBuilderPawn* Player)
{
	if (HasAuthority())
	{
		EPlaceableState OldState = PlaceableState;

		AGearPlayerState* PlayerState = IsValid(Player) ? Player->GetPlayerState<AGearPlayerState>() : nullptr;
		if (IsValid(PlayerState))
		{
			OwningPlayer = PlayerState;
			PlaceableState = EPlaceableState::Selected;
		}

		else
		{
			OwningPlayer = nullptr;
			PlaceableState = EPlaceableState::Preview;
		}

		OnRep_PlaceableState(OldState);
		
	}
}

void AGearPlaceable::SetIdle()
{
	EPlaceableState OldState = PlaceableState;

	PlaceableState = EPlaceableState::Idle;

	OnRep_PlaceableState(OldState);
}

void AGearPlaceable::SetEnabled()
{
	EPlaceableState OldState = PlaceableState;

	PlaceableState = EPlaceableState::Enabled;

	OnRep_PlaceableState(OldState);
}

bool AGearPlaceable::HasOwningPlayer() const
{
	return IsValid(OwningPlayer);
}

void AGearPlaceable::AttachToSpawnPoint(APlaceableSpawnPoint* SpawnPoint)
{
	AttachToActor(SpawnPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SetActorScale3D(FVector(PreviewScale));
	SetActorRelativeLocation(-PreviewRotationPivot->GetRelativeLocation() * PreviewScale);
}

void AGearPlaceable::OnRep_PlaceableState(EPlaceableState OldState)
{
	switch (OldState)
	{
	case EPlaceableState::None:
		break;
	case EPlaceableState::Preview:
		OnPreview_End();
		break;
	case EPlaceableState::Selected:
		OnSelected_End();
		break;
	case EPlaceableState::Idle:
		OnIdle_End();
		break;
	case EPlaceableState::Enabled:
		OnEnabled_End();
		break;
	default:
		break;
	}

	switch (PlaceableState)
	{
	case EPlaceableState::None:
		break;
	case EPlaceableState::Preview:
		OnPreview_Start();
		break;
	case EPlaceableState::Selected:
		OnSelected_Start();
		break;
	case EPlaceableState::Idle:
		OnIdle_Start();
		break;
	case EPlaceableState::Enabled:
		OnEnabled_Start();
		break;
	default:
		break;
	}
}

// ----------------------------------------------------------------------------------

void AGearPlaceable::OnPreview_Start()
{
	SetSelectionBoxEnabled(true);
}

void AGearPlaceable::OnPreview_End()
{
	SetSelectionBoxEnabled(false);
}

void AGearPlaceable::OnSelected_Start()
{
	if (IsValid(OwningPlayer))
	{
		SelectionIndicatorMaterial->SetVectorParameterValue(TEXT("Color"), OwningPlayer->PlayerColor);
		SelectionIndicator->SetVisibility(true);
	}
}

void AGearPlaceable::OnSelected_End()
{
	SelectionIndicator->SetVisibility(false);
}

void AGearPlaceable::OnIdle_Start()
{

}

void AGearPlaceable::OnIdle_End()
{

}

void AGearPlaceable::OnEnabled_Start()
{

}

void AGearPlaceable::OnEnabled_End()
{

}

void AGearPlaceable::SetSelectionBoxEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		SelectionHitbox->SetCollisionProfileName("Selectable");
	}
	else
	{
		SelectionHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}