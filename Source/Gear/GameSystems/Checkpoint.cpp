// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Checkpoint.h"
#include "GameSystems/VehicleStart.h"
#include "GameFramework/GearGameMode.h"
#include "GameFramework/GearGameState.h"
#include "Utils/GameVariablesBFL.h"
#include "Vehicle/GearVehicle.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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

	StartlineMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StartlineMesh"));
	StartlineMesh->SetupAttachment(Root);
	StartlineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StartPositionMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("StartPositionMesh"));
	StartPositionMeshes->SetupAttachment(Root);
	StartPositionMeshes->SetRelativeLocation(FVector::UpVector * 1.0f);
	StartPositionMeshes->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CheerParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CheerParticleComponent"));
	CheerParticleComponent->SetupAttachment(Root);
	CheerParticleComponent->SetAutoActivate(false);

	StartPonit_1 = CreateDefaultSubobject<UVehicleStart>(TEXT("StartPoint_1"));
	StartPonit_1->SetupAttachment(Root);

	StartPonit_2 = CreateDefaultSubobject<UVehicleStart>(TEXT("StartPoint_2"));
	StartPonit_2->SetupAttachment(Root);

	StartPonit_3 = CreateDefaultSubobject<UVehicleStart>(TEXT("StartPoint_3"));
	StartPonit_3->SetupAttachment(Root);

	StartPonit_4 = CreateDefaultSubobject<UVehicleStart>(TEXT("StartPoint_4"));
	StartPonit_4->SetupAttachment(Root);
	
	LastStartTime = FLT_MAX;
	
	StartPoint_1_Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("StartPoint_1_Arrow"));
	StartPoint_1_Arrow->SetupAttachment(StartPonit_1);

	StartPoint_2_Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("StartPoint_2_Arrow"));
	StartPoint_2_Arrow->SetupAttachment(StartPonit_2);

	StartPoint_3_Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("StartPoint_3_Arrow"));
	StartPoint_3_Arrow->SetupAttachment(StartPonit_3);

	StartPoint_4_Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("StartPoint_4_Arrow"));
	StartPoint_4_Arrow->SetupAttachment(StartPonit_4);

#if UE_BUILD_SHIPPING
	
	StartPoint_1_Arrow->MarkAsEditorOnlySubobject();
	StartPoint_2_Arrow->MarkAsEditorOnlySubobject();
	StartPoint_3_Arrow->MarkAsEditorOnlySubobject();
	StartPoint_4_Arrow->MarkAsEditorOnlySubobject();

#endif

	StartPoints.Empty(4);
	StartPoints.Add(StartPonit_1);
	StartPoints.Add(StartPonit_2);
	StartPoints.Add(StartPonit_3);
	StartPoints.Add(StartPonit_4);

	CheckpointIndex = 0;

	bReplicates = true;
	bAlwaysRelevant = true;
}

#if WITH_EDITOR
void ACheckpoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

}
#endif

void ACheckpoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACheckpoint, LastStartTime);
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();


}

void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckpoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	LapHitbox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::LapHitboxBeginOverlap);

	if (IsValid(StartlineMesh))
	{
		StartlineMaterial = StartlineMesh->CreateAndSetMaterialInstanceDynamic(0);
	}
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

void ACheckpoint::SetLightIndex(int Index)
{
	if (IsValid(StartlineMaterial))
	{
		StartlineMaterial->SetScalarParameterValue(TEXT("LightIndex"), Index);
	}
}

void ACheckpoint::SetState(int State)
{
	SetLightIndex(State);

	if (State == -1)
	{
		StartlineMesh->PlayAnimation(OpenAnimation, false);
	}

	else if (State == 4)
	{
		StartlineMesh->PlayAnimation(OpeningAnimation, false);
	}

	else
	{
		StartlineMesh->PlayAnimation(CloseAnimation, false);
	}
}

void ACheckpoint::OnRep_LastStartTime()
{
	if (LastStartTime < 0)
	{
		SetState(-1);
	}

	else
	{
		float Step = UGameVariablesBFL::GV_CountDownDuration() / 4;
		bool bInCountDown = false;

		for (int i = 1; i <= 4; i++)
		{
			float Time = LastStartTime + UGameVariablesBFL::GV_InformationPanelDuration() + i * Step;
			float TimerDelay = Time - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

			UE_LOG(LogTemp, Warning, TEXT("!!!!!!!!!!!!!!! _ %f"), TimerDelay);

			if (TimerDelay > 0)
			{
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::SetState, i), TimerDelay, false);
				bInCountDown = true;
			}
		}

		if (bInCountDown)
		{
			StartlineMesh->PlayAnimation(CloseAnimation, false);
		}
	}
}

UVehicleStart* ACheckpoint::GetVehicleStart()
{
	for (UVehicleStart* VehicleStart : StartPoints)
	{
		if (!VehicleStart->IsOccupied())
		{
			VehicleStart->MarkOccupied();
			return VehicleStart;
		}
	}

	return nullptr;
}

void ACheckpoint::ClearOccupied()
{
	for (UVehicleStart* VehicleStart : StartPoints)
	{
		VehicleStart->ClearOccupied();
	}
}

void ACheckpoint::PlayerReachedCheckpoint_Multi_Implementation(class AGearPlayerState* Player, int32 Position)
{
	if (Position == 1)
	{
		check(CheerSound);

		CheerParticleComponent->ResetSystem();
		UGameplayStatics::PlaySound2D(GetWorld(), CheerSound);
	}
}