// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle/VehicleSkidmarkComponent.h"
#include "Vehicle/GearVehicle.h"
#include "NiagaraFunctionLibrary.h"

UVehicleSkidmarkComponent::UVehicleSkidmarkComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	OwningVehicle = nullptr;
	NiagaraComponent = nullptr;
	ParticleEffect = nullptr;
	WheelIndex = 0;
}

void UVehicleSkidmarkComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningVehicle = GetOwner<AGearVehicle>();
	NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(ParticleEffect, this, NAME_None, FVector::Zero(), FRotator::ZeroRotator, FVector::OneVector, EAttachLocation::SnapToTarget, true, ENCPoolMethod::AutoRelease, true);
}

void UVehicleSkidmarkComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	check(OwningVehicle);
	check(NiagaraComponent);

	bool bWheelOnGround = OwningVehicle->IsWheelOnGround(WheelIndex);
	bool bWheelSkiding = OwningVehicle->IsWheelSkiding(WheelIndex);

	NiagaraComponent->SetFloatParameter(TEXT("User.Active"), bWheelOnGround && bWheelSkiding);
}

void UVehicleSkidmarkComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (IsValid(NiagaraComponent))
	{
		NiagaraComponent->Deactivate();
	}

	Super::OnComponentDestroyed(bDestroyingHierarchy);
}
