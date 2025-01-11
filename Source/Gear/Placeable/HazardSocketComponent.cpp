// Fill out your copyright notice in the Description page of Project Settings.

#include "Placeable/HazardSocketComponent.h"
#include "GameFramework/GearBuilderPawn.h"
#include "GameFramework/GearPlayerController.h"
#include "Net/UnrealNetwork.h"

UHazardSocketComponent::UHazardSocketComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SocketType = EHazardSocketType::SmallHazard;
	bOccupied = false;

	SetIsReplicatedByDefault(true);
}

void UHazardSocketComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UHazardSocketComponent, SocketType, COND_InitialOnly);
	DOREPLIFETIME(UHazardSocketComponent, bOccupied);
}

void UHazardSocketComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UHazardSocketComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UHazardSocketComponent::OnRep_Occupied()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (IsValid(PlayerController) && PlayerController->IsLocalController())
		{			
			AGearBuilderPawn* BuilderPawn = PlayerController->GetPawn<AGearBuilderPawn>();
			if (IsValid(BuilderPawn))
			{
				BuilderPawn->UpdateHazardMarkers();
			}
		}
	}
}

bool UHazardSocketComponent::IsOccupied() const
{
	return bOccupied;
}

void UHazardSocketComponent::MarkOccupied()
{
	bOccupied = true;
}