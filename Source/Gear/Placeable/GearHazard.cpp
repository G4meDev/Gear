// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/GearHazard.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"


AGearHazard::AGearHazard()
{
	PrimaryActorTick.bCanEverTick = true;

	PlacingCount = 2;
}

void AGearHazard::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

// void AGearHazard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
// 
// 
// 	
// }

#if WITH_EDITOR

void AGearHazard::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName ProperyName = PropertyChangedEvent.Property == nullptr ? NAME_None : PropertyChangedEvent.Property->GetFName();

	if (ProperyName == GET_MEMBER_NAME_CHECKED(AGearHazard, bDirty))
	{
		bDirty = false;
	}
}

#endif

void AGearHazard::BeginPlay()
{
	Super::BeginPlay();


}

void AGearHazard::SetPreview()
{
	Super::SetPreview();
}

void AGearHazard::SetSelectedBy(AGearBuilderPawn* Player)
{
	Super::SetSelectedBy(Player);
}

void AGearHazard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}

// ---------------------------------------------------------------------------------------------------------

void AGearHazard::OnIdle_Start()
{
	Super::OnIdle_Start();

	Idle_Start();
}

void AGearHazard::OnIdle_End()
{
	Super::OnIdle_End();

	Idle_End();
}

void AGearHazard::OnEnabled_Start()
{
	Super::OnEnabled_Start();

	Enabled_Start();
}

void AGearHazard::OnEnabled_End()
{
	Super::OnEnabled_End();

	Enabled_End();
}