// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkDiscoveryActor.h"
#include "Online/NBoSerializer.h"
#include "SocketSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GearGameInstance.h"

#define LAN_DISCOVERY_NONCE 1

ANetworkDiscoveryActor::ANetworkDiscoveryActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ANetworkDiscoveryActor::BeginPlay()
{
	Super::BeginPlay();
	
	LanSession.LanNonce = LAN_DISCOVERY_NONCE;
}

void ANetworkDiscoveryActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LanSession.Tick(DeltaTime);
}

void ANetworkDiscoveryActor::StartDiscovery(bool bHost)
{
	UE_LOG(LogTemp, Log, TEXT("discovery started as %s"), bHost ? TEXT("host") : TEXT("client"));

	if (bHost)
	{
		auto del = FOnValidQueryPacketDelegate::CreateUObject(this, &ANetworkDiscoveryActor::OnValidQueryPacket);
		LanSession.Host(del);
	}

	else
	{
		FNboSerializeToBuffer Packet(LAN_BEACON_MAX_PACKET_SIZE);
		LanSession.CreateClientQueryPacket(Packet, LAN_DISCOVERY_NONCE);

		auto ValidQueryDel = FOnValidResponsePacketDelegate::CreateUObject(this, &ANetworkDiscoveryActor::OnValidResponsePacket);
		auto TimeoutDel = FOnSearchingTimeoutDelegate::CreateUObject(this, &ANetworkDiscoveryActor::OnSearchTimeout);

		LanSession.Search(Packet, ValidQueryDel, TimeoutDel);
	}
}

void ANetworkDiscoveryActor::StopDiscovery()
{
	LanSession.StopLANSession();
}

void ANetworkDiscoveryActor::OnValidQueryPacket(uint8* Bytes, int32 Length, uint64 Nonce)
{
 	FNboSerializeToBuffer Packet(LAN_BEACON_MAX_PACKET_SIZE);
 	LanSession.CreateHostResponsePacket(Packet, Nonce);

	UGearGameInstance* GearGameInstance = Cast<UGearGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FString HostName = GearGameInstance->GetPlayerName();

	bool bCanBindAll;
	TSharedPtr<class FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll);
	FString IP = Addr->ToString(false);

	Packet << HostName;
	Packet << IP;

 	LanSession.BroadcastPacket(Packet, Packet.GetByteCount());

	UE_LOG(LogTemp, Log, TEXT("send response with hostname: %s and ip: %s"), *HostName, *IP);
}

void ANetworkDiscoveryActor::OnValidResponsePacket(uint8* Bytes, int32 Length)
{
	FString HostName;
	FString IP;

	FNboSerializeFromBuffer PacketReader(Bytes, Length);
	PacketReader >> HostName;
	PacketReader >> IP;

	UE_LOG(LogTemp, Log, TEXT("recived response from hostname: %s and ip: %s"), *HostName, *IP);

	FGearHostInfo HostInfo = FGearHostInfo(HostName, IP);
	OnFoundHostDelegate.Broadcast(HostInfo);
}

void ANetworkDiscoveryActor::OnSearchTimeout()
{
	UE_LOG(LogTemp, Log, TEXT("discovery search time out"));

	StopDiscovery();
}

#undef LAN_DISCOVERY_NONCE