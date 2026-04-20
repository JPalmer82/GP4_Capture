// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Framework/CGameSession.h"
#include "EngineUtils.h"

ACGameMode::ACGameMode()
{
	GameSessionClass = ACGameSession::StaticClass();
}

APlayerController* ACGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);
	FGenericTeamId TeamId = GetTeamIdForPlayer(NewPlayerController);

	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController);

	if (NewPlayerTeamInterface)
	{
		NewPlayerTeamInterface->SetGenericTeamId(TeamId);
	}

	NewPlayerController->StartSpot = GetStartSpotForTeam(TeamId);
	return NewPlayerController;
}

FGenericTeamId ACGameMode::GetTeamIdForPlayer(const APlayerController* PlayerController)
{
	static int TeamId = 0;
	++TeamId;
	return FGenericTeamId(TeamId % 2);
}

AActor* ACGameMode::GetStartSpotForTeam(FGenericTeamId TeamId) const
{
	const FName* TeamPlayerStartTag = TeamStartSpotTagMap.Find(TeamId);
	if (!TeamPlayerStartTag)
	{
		return nullptr;
	}

	for (TActorIterator<APlayerStart> Iter(GetWorld()); Iter; ++Iter)
	{
		APlayerStart* PlayerStartActor = *Iter;
		if (PlayerStartActor->PlayerStartTag == *TeamPlayerStartTag)
		{
			PlayerStartActor->PlayerStartTag = "Taken";
			return *Iter;
		}
	}

	return nullptr;
}
