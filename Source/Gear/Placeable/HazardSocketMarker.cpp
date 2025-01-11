// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/HazardSocketMarker.h"
#include "Components/BoxComponent.h"
#include "GameFramework/GearPlayerController.h"

AHazardSocketMarker::AHazardSocketMarker()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SelectionHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("SelectionHitbox"));
	SelectionHitbox->SetupAttachment(Root);
	SelectionHitbox->SetCollisionProfileName("Selectable");
}

void AHazardSocketMarker::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SelectionHitbox->OnClicked.AddDynamic(this, &AHazardSocketMarker::OnSelectionBoxClicked);
	SelectionHitbox->OnInputTouchBegin.AddDynamic(this, &AHazardSocketMarker::OnSelectionBoxTouched);
}

void AHazardSocketMarker::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHazardSocketMarker::Destroyed()
{
	Super::Destroyed();

	TargetSocket = nullptr;
}

void AHazardSocketMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHazardSocketMarker::SelectionBoxClicked()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			AGearPlayerController* GearController = Cast<AGearPlayerController>(PlayerController);
			if (IsValid(GearController))
			{
				GearController->ClickedOnHazardSocketMarker(this);
			}
		}
	}
}

void AHazardSocketMarker::OnSelectionBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	SelectionBoxClicked();
}

void AHazardSocketMarker::OnSelectionBoxTouched(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent)
{
	SelectionBoxClicked();
}