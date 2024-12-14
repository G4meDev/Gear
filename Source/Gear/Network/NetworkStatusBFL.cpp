// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/NetworkStatusBFL.h"
#include "Gameframework/PlayerState.h"

FNetworkStatus UNetworkStatusBFL::GetNetworkStatus(APlayerController* PC)
{
	FNetworkStatus NS;

	if (IsValid(PC))
	{
		UNetConnection* NetConnection = PC->NetConnection;
		if (IsValid(NetConnection))
		{
			NS.OutPacketsPerSecond	= NetConnection->OutPacketsPerSecond;
			NS.OutBytesPerSecond	= NetConnection->OutBytesPerSecond;
			NS.OutPacketsLost		= NetConnection->OutPacketsLost;

			NS.InPacketsPerSecond	= NetConnection->InPacketsPerSecond;
			NS.InBytesPerSecond		= NetConnection->InBytesPerSecond;
			NS.InPacketsLost		= NetConnection->InPacketsLost;
		}

		APlayerState* PlayerState = PC->GetPlayerState<APlayerState>();
		if (IsValid(PlayerState))
		{
			NS.Ping = PlayerState->GetPingInMilliseconds();
		}
	}

	return NS;
}
