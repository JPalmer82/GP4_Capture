// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameMode.h"

APlayerController* ACGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);
	FGenericTeamId TeamId = GetTeamIdForPlayer(NewPlayerController);

	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController);

	if (NewPlayerTeamInterface)
	{
		NewPlayerTeamInterface->SetGenericTeamId(TeamId);
	}

	return NewPlayerController;
}

FGenericTeamId ACGameMode::GetTeamIdForPlayer(const APlayerController* PlayerController)
{
	static int TeamId = 0;
	++TeamId;
	return FGenericTeamId(TeamId % 2);
}
