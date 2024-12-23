// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Checkpoint.h"
#include "GameSystems/VehicleStart.h"
#include "GameFramework/GearGameMode.h"
#include "Vehicle/GearVehicle.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	LapHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("LapHitbox"));
	LapHitbox->SetupAttachment(Root);
	LapHitbox->SetGenerateOverlapEvents(true);
	LapHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LapHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
	LapHitbox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECR_Overlap);

	StartLineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StartLineMesh"));
	StartLineMesh->SetupAttachment(Root);
	StartLineMesh->SetRelativeLocation(FVector::UpVector * 1.0f);
	StartLineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StartPositionMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("StartPositionMesh"));
	StartPositionMeshes->SetupAttachment(Root);
	StartPositionMeshes->SetRelativeLocation(FVector::UpVector * 1.0f);
	StartPositionMeshes->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh"));
	HandMesh->SetupAttachment(Root);
	HandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StartPonit_1 = CreateDefaultSubobject<UVehicleStart>(TEXT("StartPoint_1"));
	StartPonit_1->SetupAttachment(Root);

	StartPonit_2 = CreateDefaultSubobject<UVehicleStart>(TEXT("StartPoint_2"));
	StartPonit_2->SetupAttachment(Root);

	StartPonit_3 = CreateDefaultSubobject<UVehicleStart>(TEXT("StartPoint_3"));
	StartPonit_3->SetupAttachment(Root);

	StartPonit_4 = CreateDefaultSubobject<UVehicleStart>(TEXT("StartPoint_4"));
	StartPonit_4->SetupAttachment(Root);

#if WITH_EDITORONLY_DATA
	
	StartPoint_1_Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("StartPoint_1_Arrow"));
	StartPoint_1_Arrow->SetupAttachment(StartPonit_1);

	StartPoint_2_Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("StartPoint_2_Arrow"));
	StartPoint_2_Arrow->SetupAttachment(StartPonit_2);

	StartPoint_3_Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("StartPoint_3_Arrow"));
	StartPoint_3_Arrow->SetupAttachment(StartPonit_3);

	StartPoint_4_Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("StartPoint_4_Arrow"));
	StartPoint_4_Arrow->SetupAttachment(StartPonit_4);

#endif

	StartPoints.Empty(4);
	StartPoints.Add(StartPonit_1);
	StartPoints.Add(StartPonit_2);
	StartPoints.Add(StartPonit_3);
	StartPoints.Add(StartPonit_4);

	Init(200.0f, 300.0f, 750.0f, 0.65f, 500.0f);

	CheckpointIndex = 0;

	bReplicates = true;
	bAlwaysRelevant = true;
}

void ACheckpoint::Init(float InWidth, float InHeight, float InLength, float InLateralSeperationRatio, float InLongitudinalSeperation)
{
	StartPositionMeshes->ClearInstances();

	Width = InWidth;
	Height = InHeight;
	Length = InLength;

	LateralSeperationRatio = InLateralSeperationRatio;
	LongitudinalSeperation = InLongitudinalSeperation;

	StartLineMesh->SetRelativeScale3D(FVector(Width, Length, 100.0f) / 100.0f);
	LapHitbox->SetBoxExtent(FVector(Width, Length, Height) / 2);
	LapHitbox->SetRelativeLocation(FVector::UpVector * LapHitbox->GetUnscaledBoxExtent().Z);

	const float LateralSeperation = FMath::Lerp(0, Length/2, LateralSeperationRatio);
	for (int i = 0; i < StartPoints.Num(); i++)
	{
		bool bEvenIndex = i % 2 == 0;
		float Lat = bEvenIndex ? -LateralSeperation : LateralSeperation;
		
		int NumRow = i/2;
		float Long = (NumRow * LongitudinalSeperation) + (0.5f * LongitudinalSeperation) + (0.5f * Width);

		FVector InstancePos = FVector(-Long, Lat, 0.0f);
		StartPoints[i]->SetRelativeLocation(InstancePos);
		
		StartPositionMeshes->AddInstance(FTransform(InstancePos));
	}
}

#if WITH_EDITOR
void ACheckpoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName ProperyName = PropertyChangedEvent.Property == nullptr ? NAME_None : PropertyChangedEvent.Property->GetFName(); 
	
	if (ProperyName == GET_MEMBER_NAME_CHECKED(ACheckpoint, LateralSeperationRatio))
	{
		LateralSeperationRatio = FMath::Clamp(LateralSeperationRatio, 0, 1);
	}
	
	if (
		ProperyName == GET_MEMBER_NAME_CHECKED(ACheckpoint, Width)
		|| ProperyName == GET_MEMBER_NAME_CHECKED(ACheckpoint, Height)
		|| ProperyName == GET_MEMBER_NAME_CHECKED(ACheckpoint, Length)
		|| ProperyName == GET_MEMBER_NAME_CHECKED(ACheckpoint, LateralSeperationRatio)
		|| ProperyName == GET_MEMBER_NAME_CHECKED(ACheckpoint, LongitudinalSeperation)
		)
	{
		Init(Width, Height, Length, LateralSeperationRatio, LongitudinalSeperation);
	}
}
#endif

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();


}

void ACheckpoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	LapHitbox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::LapHitboxBeginOverlap);

}

void ACheckpoint::LapHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (HasAuthority() && CheckpointIndex != 0)
	{
		AGearVehicle* GearVehicle = Cast<AGearVehicle>(OtherActor);
		if (IsValid(GearVehicle) && GearVehicle->TargetCheckpoint == CheckpointIndex)
		{
			AGearGameMode* GearGameMode = GetWorld()->GetAuthGameMode<AGearGameMode>();
			check(GearGameMode);
			
			GearGameMode->VehicleReachedCheckpoint(GearVehicle, this);
		}
	}
}

void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}