// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameSession.h"
#include "Framework/CGameInstance.h"

bool ACGameSession::ProcessAutoLogin()
{
	return true;
}

void ACGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);
	GetGameInstance<UCGameInstance>()->PlayerJoined(UniqueId);
}

void ACGameSession::UnregisterPlayer(FName FromSessionName, const FUniqueNetIdRepl& UniqueId)
{
	Super::UnregisterPlayer(FromSessionName, UniqueId);
	GetGameInstance<UCGameInstance>()->PlayerLeft(UniqueId);
}
