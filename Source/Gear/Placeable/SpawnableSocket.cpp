// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeable/SpawnableSocket.h"

USpawnableSocket::USpawnableSocket()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void USpawnableSocket::BeginPlay()
{
	Super::BeginPlay();

	
}


void USpawnableSocket::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}