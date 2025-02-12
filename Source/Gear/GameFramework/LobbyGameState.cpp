// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/LobbyGameState.h"
#include "GameFramework/LobbyPlayerController.h"
#include "GameFramework/LobbyPlayerState.h"
#include "GameFramework/LobbyGameMode.h"
#include "GameSystems/LobbyPlayerPlatform.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ALobbyGameState::ALobbyGameState()
{

	LobbyGameState = ELobbyGameState::WaitingForPlayers;
	NumAllowedPlayers = 4;
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyGameState, NumAllowedPlayers);
}

void ALobbyGameState::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> PlatformActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyPlayerPlatform::StaticClass(), PlatformActors);

	PlayerPlatforms.Reserve(4);
	for (AActor* PlatformActor : PlatformActors)
	{
		ALobbyPlayerPlatform* PlayerPlatform = Cast<ALobbyPlayerPlatform>(PlatformActor);
		if (IsValid(PlayerPlatform))
		{
			PlayerPlatforms.Add(PlayerPlatform);
		}
	}

	PlayerPlatforms.StableSort([](const ALobbyPlayerPlatform& P1, const ALobbyPlayerPlatform& P2)
		{
			return P1.GetPlatformIndex() < P2.GetPlatformIndex();
		});

	ReconstructPlayersPlatform();
}

void ALobbyGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority() && LobbyGameState == ELobbyGameState::WaitingForPlayers && CanStartGame())
	{
		if (GetWorld())
		{
			LobbyGameState = ELobbyGameState::StartingGame;
			GetWorld()->GetTimerManager().SetTimer(StartGameTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::StartGame), 3.0f, false);
			
			StartGame_Multi(GetServerWorldTimeSeconds());
		}
	}

	else if(HasAuthority() && LobbyGameState == ELobbyGameState::StartingGame && !CanStartGame())
	{
		if (GetWorld())
		{
			LobbyGameState = ELobbyGameState::WaitingForPlayers;
			GetWorld()->GetTimerManager().ClearTimer(StartGameTimerHandle);
			StartGameTimerHandle.Invalidate();

			StartGameCancel_Multi();
		}
	}
}

void ALobbyGameState::GetInUseColors(TArray<EPlayerColorCode>& InUseColors)
{
	for (APlayerState* Player : PlayerArray)
	{
		ALobbyPlayerState* LobbyPlayer = Cast<ALobbyPlayerState>(Player);
		if (IsValid(LobbyPlayer))
		{
			InUseColors.Add(LobbyPlayer->ColorCode);
		}
	}
}

void ALobbyGameState::AssignNewColorToPlayer(APlayerState* Player)
{
	ALobbyPlayerState* LobbyPlayer = Cast<ALobbyPlayerState>(Player);
	if (IsValid(LobbyPlayer))
	{
		TArray<EPlayerColorCode> InUseColors;
		GetInUseColors(InUseColors);
		InUseColors.RemoveSingle(LobbyPlayer->ColorCode);

		for (uint8 i = 0; i < static_cast<uint8>(EPlayerColorCode::MAX_COLOR); i++)
		{
			EPlayerColorCode ColorCode = static_cast<EPlayerColorCode>(i);

			if (!InUseColors.Contains(ColorCode))
			{
				LobbyPlayer->ColorCode = ColorCode;
				LobbyPlayer->OnRep_ColorCode();
				return;
			}
		}
	}
}

bool ALobbyGameState::CanStartGame()
{
	ALobbyGameMode* LobbyGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ALobbyGameMode>() : nullptr;
	if (IsValid(LobbyGameMode) && LobbyGameMode->GetNumPlayers() > 1 && LobbyGameMode->NumTravellingPlayers == 0)
	{

		TArray<ALobbyPlayerState*> LobbyPlayers = GetLobbyPlayers();
		bool bEveryPlayerIsReady = true;

		for (ALobbyPlayerState* LobbyPlayer : LobbyPlayers)
		{
			if (IsValid(LobbyPlayer) && !LobbyPlayer->IsReady())
			{
				bEveryPlayerIsReady = false;
				break;
			}
		}

		return bEveryPlayerIsReady;
	}

	return false;
}

void ALobbyGameState::StartGame()
{
	GetWorld()->ServerTravel("TestMap", true);
}

void ALobbyGameState::StartGame_Multi_Implementation(float StartTime)
{
	ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(LobbyPlayerController))
	{
		LobbyPlayerController->OnStartGame(StartTime);
	}
}

void ALobbyGameState::StartGameCancel_Multi_Implementation()
{
	ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(LobbyPlayerController))
	{
		LobbyPlayerController->OnStartGameCancel();
	}
}

void ALobbyGameState::OnRep_NumAllowedPlayers()
{
	OnNumberOfAllowedPlayerChanged.Broadcast(NumAllowedPlayers);
}

void ALobbyGameState::RequestColorChangeForPlayer(ALobbyPlayerController* PC, EPlayerColorCode Color)
{
	ALobbyPlayerState* Player = Cast<ALobbyPlayerState>(PC->PlayerState);
	if (!IsValid(Player))
	{
		return;
	}

	TArray<EPlayerColorCode> InUseColors;
	GetInUseColors(InUseColors);

	if (!InUseColors.Contains(Color))
	{
		Player->ColorCode = Color;
		Player->OnRep_ColorCode();
	}

	else
	{
		PC->ColorChange_RollBack(Player->ColorCode);
	}
}

TArray<ALobbyPlayerState*> ALobbyGameState::GetLobbyPlayers()
{
	TArray<ALobbyPlayerState*> LobbyPlayerStates;
	for (APlayerState* PlayerState : PlayerArray)
	{
		ALobbyPlayerState* LobbyPlayerState = Cast<ALobbyPlayerState>(PlayerState);
		if (IsValid(LobbyPlayerState))
		{
			LobbyPlayerStates.Add(LobbyPlayerState);
		}
	}

	return LobbyPlayerStates;
}

TArray<ALobbyPlayerState*> ALobbyGameState::GetJoinTimeSortedLobbyPlayers()
{
	TArray<ALobbyPlayerState*> LobbyPlayerStates = GetLobbyPlayers();
	LobbyPlayerStates.StableSort(ALobbyPlayerState::SortJoinTimePredicate);
	
	return LobbyPlayerStates;
}

void ALobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	ReconstructPlayersPlatform();

	if (HasAuthority())
	{
		AssignNewColorToPlayer(PlayerState);
	}


	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			ALobbyPlayerController* LobbyController = Cast<ALobbyPlayerController>(PlayerController);

			if (IsValid(LobbyController))
			{
				LobbyController->NotifyNewPlayer(PlayerState);
			}
		}
	}
}

void ALobbyGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	ALobbyPlayerState* LobbyPlayerState = Cast<ALobbyPlayerState>(PlayerState);
	if (IsValid(LobbyPlayerState))
	{
		LobbyPlayerState->PlayerJoinTime = GetServerWorldTimeSeconds();
	}

	ReconstructPlayersPlatform();

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->IsLocalController())
		{
			ALobbyPlayerController* LobbyController = Cast<ALobbyPlayerController>(PlayerController);

			if (IsValid(LobbyController))
			{
				LobbyController->NotifyRemovePlayer(PlayerState);
			}
		}
	}
}

void ALobbyGameState::ReconstructPlayersPlatform()
{
	TArray<ALobbyPlayerState*> LobbyPlayers = GetJoinTimeSortedLobbyPlayers();

	for (int i = 0; i < PlayerPlatforms.Num(); i++)
	{
		ALobbyPlayerPlatform* PlayerPlatform = PlayerPlatforms[i];
		if (IsValid(PlayerPlatform))
		{
			ALobbyPlayerState* PlayerState = i < LobbyPlayers.Num() ? LobbyPlayers[i] : nullptr;
			PlayerPlatform->SetOwningPlayer(PlayerState);
		}
	}
}

bool ALobbyGameState::IsWaitingForPlayers()
{
	return LobbyGameState == ELobbyGameState::WaitingForPlayers;
}

void ALobbyGameState::SetNumAllowedPlayers(int32 InAllowedNumberOfPlayers)
{
	if (HasAuthority())
	{
		ALobbyGameMode* LobbyGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ALobbyGameMode>() : nullptr;
		if (IsValid(LobbyGameMode))
		{
			if (InAllowedNumberOfPlayers >= LobbyGameMode->GetNumPlayers() && InAllowedNumberOfPlayers <= 4)
			{
				NumAllowedPlayers = InAllowedNumberOfPlayers;
			}
		}

		OnRep_NumAllowedPlayers();
	}
}