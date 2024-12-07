// Fill out your copyright notice in the Description page of Project Settings.


#include "GearCameraManager.h"
#include "GameFramework/GearGameState.h"
#include "Vehicle/GearVehicle.h"
#include "GameSystems/TrackSpline.h"


void AGearCameraManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

// 	APlayerController* PlayerController = GetOwningPlayerController();
// 	AGearVehicle* GearVehicle = PlayerController ? PlayerController->GetPawn<AGearVehicle>() : nullptr;
// 
// 	if (IsValid(GearVehicle) && IsValid(GearGameState) && IsValid(GearGameState->TrackSpline))
// 	{
// 		FCrossTrackProperty CrossTrackProperty;
// 		GearGameState->TrackSpline->GetTrackPropertiesAtLocation(GearVehicle->GetActorLocation(), CrossTrackProperty);
// 		
// 		FVector E = GearVehicle->GetActorLocation() + FVector::UpVector * 500;
// 
// 		DrawDebugLine(GetWorld(), E, E + CrossTrackProperty.Tangent * 100, FColor::Red, false, 0.1f);
// 		DrawDebugLine(GetWorld(), E, E + CrossTrackProperty.Right * 100, FColor::Green, false, 0.1f);
// 		DrawDebugLine(GetWorld(), E, E + CrossTrackProperty.Up * 100, FColor::Blue, false, 0.1f);
// 
// 		DrawDebugLine(GetWorld(), E, E + CrossTrackProperty.FlattenRight * CrossTrackProperty.Error, FColor::Black, false, 0.1f);
// 	}

}

void AGearCameraManager::BeginPlay()
{
	Super::BeginPlay();

}

void AGearCameraManager::InitializeFor(class APlayerController* PC)
{
	Super::InitializeFor(PC);

	GearGameState = GetWorld()->GetGameState<AGearGameState>();
}

void AGearCameraManager::Destroyed()
{
	Super::Destroyed();


}