// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ScoreboardWidget.h"
#include "UI/ScoreboardEntryWidget.h"
#include "GameFramework/GearHUD.h"
#include "GameFramework/GearPlayerState.h"
#include "Kismet/GameplayStatics.h"

void UScoreboardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(OwningHUD))
	{
		OwningHUD->OnPlayerJoined.AddDynamic(this, &UScoreboardWidget::AddPlayer);
		OwningHUD->OnPlayerQuit.AddDynamic(this, &UScoreboardWidget::RemovePlayer);
	}

	Entry_1->InitWidget(this);
	Entry_2->InitWidget(this);
	Entry_3->InitWidget(this);
	Entry_4->InitWidget(this);

	TArray<AActor*> PlayerActors;
	UGameplayStatics::GetAllActorsOfClass(this, AGearPlayerState::StaticClass(), PlayerActors);

	for (AActor* PlayerActor : PlayerActors)
	{
		AGearPlayerState* Player = Cast<AGearPlayerState>(PlayerActor);
		if (IsValid(Player))
		{
			AddPlayer(Player);
		}
	}
}

void UScoreboardWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UScoreboardWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(OwningHUD))
	{
		OwningHUD->OnPlayerJoined.RemoveDynamic(this, &UScoreboardWidget::AddPlayer);
		OwningHUD->OnPlayerQuit.RemoveDynamic(this, &UScoreboardWidget::RemovePlayer);
	}
}

void UScoreboardWidget::AddPlayer(AGearPlayerState* Player)
{
	auto TryAddPlayer = [&](UScoreboardEntryWidget* Entry)
		{
			if (IsValid(Entry) && !IsValid(Entry->Player))
			{
				Entry->Player = Player;
				return true;
			}

			return false;
		};

	if (IsValid(Player))
	{
		if (TryAddPlayer(Entry_1))
			return;

		if (TryAddPlayer(Entry_2))
			return;

		if (TryAddPlayer(Entry_3))
			return;

		if (TryAddPlayer(Entry_4))
			return;
	}
}

void UScoreboardWidget::RemovePlayer(AGearPlayerState* Player)
{
	auto TryRemovePlayer = [&](UScoreboardEntryWidget* Entry)
		{
			if (IsValid(Entry) && Entry->Player == Player)
			{
				Entry->Player = nullptr;
			}
		};

	if (IsValid(Player))
	{
		TryRemovePlayer(Entry_1);
		TryRemovePlayer(Entry_2);
		TryRemovePlayer(Entry_3);
		TryRemovePlayer(Entry_4);
	}
}