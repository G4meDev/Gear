// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GearBuilderPawn.h"

AGearBuilderPawn::AGearBuilderPawn()
{
	PrimaryActorTick.bCanEverTick = true;


}

void AGearBuilderPawn::BeginPlay()
{
	Super::BeginPlay();


	
}

void AGearBuilderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGearBuilderPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

