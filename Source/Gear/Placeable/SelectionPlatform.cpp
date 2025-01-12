// Fill out your copyright notice in the Description page of Project Settings.

#include "Placeable/SelectionPlatform.h"

ASelectionPlatform::ASelectionPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(Root);
	PlatformMesh->SetCollisionProfileName(TEXT("NoCollision"));

	Socket_1 = CreateDefaultSubobject<USceneComponent>(TEXT("Socket_1"));
	Socket_1->SetupAttachment(Root);

	Socket_2 = CreateDefaultSubobject<USceneComponent>(TEXT("Socket_2"));
	Socket_2->SetupAttachment(Root);

	Socket_3 = CreateDefaultSubobject<USceneComponent>(TEXT("Socket_3"));
	Socket_3->SetupAttachment(Root);

	Socket_4 = CreateDefaultSubobject<USceneComponent>(TEXT("Socket_4"));
	Socket_4->SetupAttachment(Root);

	Socket_5 = CreateDefaultSubobject<USceneComponent>(TEXT("Socket_5"));
	Socket_5->SetupAttachment(Root);

	Sockets.Reserve(5);
	Sockets.Add(Socket_1);
	Sockets.Add(Socket_2);
	Sockets.Add(Socket_3);
	Sockets.Add(Socket_4);
	Sockets.Add(Socket_5);

	bReplicates = true;
	bAlwaysRelevant = true;
}

void ASelectionPlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASelectionPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}