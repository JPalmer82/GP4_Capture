// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSessionSettings.h"

void UCGameInstance::Init()
{
	Super::Init();

	if (GetWorld()->IsEditorWorld())
	{
		return;
	}

	if (GetIsSessionServer())
	{
		CreateSession();
	}
}

bool UCGameInstance::GetIsSessionServer() const
{
	return GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer;
}

FString UCGameInstance::GetSessionNameStr() const
{
	return GetCommandLineArgAsString(GetSessionNameKey());
}

int UCGameInstance::GetSessionPort() const
{
	return GetCommandLineArgAsInt(GetSessionPortKey());
}

FString UCGameInstance::GetSessionSearchId() const
{
	return GetCommandLineArgAsString(GetSessionSearchIDKey());
}

FName UCGameInstance::GetSessionNameKey() const
{
	return FName("SESSION_NAME");
}

FName UCGameInstance::GetSessionPortKey() const
{
	return FName("PORT");
}

FName UCGameInstance::GetSessionSearchIDKey() const
{
	return FName("SESSION_SEARCH_ID");
}

FString UCGameInstance::GetCommandLineArgAsString(const FName& ParamName) const
{
	FString OutVal = "";
	FString CommandLineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

int UCGameInstance::GetCommandLineArgAsInt(const FName& ParamName) const
{
	int OutVal = 0;
	FString CommandLineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

IOnlineSessionPtr UCGameInstance::GetSessionPtr() const
{
	IOnlineSubsystem* OnlineSubSystem = IOnlineSubsystem::Get();
	if (OnlineSubSystem)
	{
		return OnlineSubSystem->GetSessionInterface();
	}

	return nullptr;
}

IOnlineIdentityPtr UCGameInstance::GetIdentityPtr() const
{
	IOnlineSubsystem* OnlineSubSystem = IOnlineSubsystem::Get();
	if (OnlineSubSystem)
	{
		return OnlineSubSystem->GetIdentityInterface();
	}

	return nullptr;
}

void UCGameInstance::PlayerJoined(const FUniqueNetIdRepl& UniqueId)
{
	PlayerRecord.AddUnique(UniqueId);
	CancelSessionTermination();
}

void UCGameInstance::PlayerLeft(const FUniqueNetIdRepl& UniqueId)
{
	PlayerRecord.Remove(UniqueId);
	if (PlayerRecord.Num() == 0)
	{
		StartSessionTermination();
	}
}

void UCGameInstance::CreateSession()
{
	ServerSessionName = GetSessionNameStr();
	ServerPort = GetSessionPort();
	FString SearchID = GetSessionSearchId();
	UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------------"))
	UE_LOG(LogTemp, Warning, TEXT("Server Session Name: %s"), *(ServerSessionName))
	UE_LOG(LogTemp, Warning, TEXT("Server Port: %d"), ServerPort)
	UE_LOG(LogTemp, Warning, TEXT("Server Search ID: %s"), *(SearchID))
	UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------------"))

	IOnlineSessionPtr OnlineSessionPtr = GetSessionPtr();
	if (OnlineSessionPtr)
	{
		FOnlineSessionSettings OnlineSessionSettings;
		OnlineSessionSettings.bIsLANMatch = false;
		OnlineSessionSettings.NumPublicConnections = 10;
		OnlineSessionSettings.bShouldAdvertise = true;

		OnlineSessionSettings.bUsesPresence = false;
		OnlineSessionSettings.bAllowJoinViaPresence = false;
		OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;

		OnlineSessionSettings.bAllowInvites = true;
		OnlineSessionSettings.bAllowJoinInProgress = true;

		OnlineSessionSettings.bUseLobbiesIfAvailable = false;
		OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;

		OnlineSessionSettings.bUsesStats = true;

		OnlineSessionSettings.Set(GetSessionNameKey(), ServerSessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		OnlineSessionSettings.Set(GetSessionSearchIDKey(), SearchID, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		OnlineSessionSettings.Set(GetSessionPortKey(), ServerPort, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		OnlineSessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		OnlineSessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UCGameInstance::SessionCreated);
		if (!OnlineSessionPtr->CreateSession(0, FName(ServerSessionName), OnlineSessionSettings))
		{
			OnlineSessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
			UE_LOG(LogTemp, Warning, TEXT("Session Creation Failed Right Away."))
			FGenericPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UCGameInstance::SessionCreated(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"))
		UE_LOG(LogTemp, Warning, TEXT("Session With Name : %s"), *(SessionName.ToString()))
		UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"))
		LoadMapAndListen(GameMap);

		StartSessionTermination();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"))
		UE_LOG(LogTemp, Warning, TEXT("Session did not get created"))
		UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------"))
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UCGameInstance::StartSessionTermination()
{
	GetWorld()->GetTimerManager().SetTimer(SessionTerminationTimerHandler, this, &UCGameInstance::TerminateSession, SessionTerminationWaitTime);
}

void UCGameInstance::CancelSessionTermination()
{
	GetWorld()->GetTimerManager().ClearTimer(SessionTerminationTimerHandler);
}

void UCGameInstance::TerminateSession()
{
	IOnlineSessionPtr OnlineSessionPtr = GetSessionPtr();
	if (OnlineSessionPtr)
	{
		OnlineSessionPtr->OnEndSessionCompleteDelegates.RemoveAll(this);
		OnlineSessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnSessionEnded);
		OnlineSessionPtr->EndSession(FName(ServerSessionName));
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UCGameInstance::OnSessionEnded(FName SessionName, bool bWasSuccessful)
{
	FGenericPlatformMisc::RequestExit(false);
}

void UCGameInstance::LoadMapAndListen(TSoftObjectPtr<UWorld> Level)
{
	const FName LevelURL = (*FPackageName::ObjectPathToPackageName(Level.ToString()));
	if (LevelURL != "")
	{
		FString TravelStr = FString::Printf(TEXT("%s?Listen?port=%d"), *(LevelURL.ToString()), ServerPort);
		UE_LOG(LogTemp, Warning, TEXT("travel str is: %s"), *(TravelStr))
		GetWorld()->ServerTravel(TravelStr);
	}
}

void UCGameInstance::ClientAuthPortalLogin()
{
	IOnlineIdentityPtr OnlineIdentityPointer = GetIdentityPtr();

	if (OnlineIdentityPointer)
	{
		if (OnlineIdentityPointer->Login(0, FOnlineAccountCredentials("accountportal", "", "")))
		{
			OnlineIdentityPointer->OnLoginCompleteDelegates->AddUObject(this, &UCGameInstance::ClientLoginCompleted);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("--------------------------------------------------"))
		UE_LOG(LogTemp, Warning, TEXT("Login Failed right away"))
		UE_LOG(LogTemp, Warning, TEXT("--------------------------------------------------"))
	}
}

void UCGameInstance::ClientLoginCompleted(int LocalPlayerNumber, bool bWasSuccessful, const FUniqueNetId& UserID, const FString& ErrorMsg)
{
	if (bWasSuccessful)
	{
		IOnlineIdentityPtr OnlineIdentityPointer = GetIdentityPtr();

		if (OnlineIdentityPointer)
		{
			UE_LOG(LogTemp, Warning, TEXT("--------------------------------------------------"))
				UE_LOG(LogTemp, Warning, TEXT("Login Was Successful with player nickname: %s"), *(OnlineIdentityPointer->GetPlayerNickname(UserID)))
				UE_LOG(LogTemp, Warning, TEXT("--------------------------------------------------"))
				OnLoginCompleted.Broadcast(bWasSuccessful, OnlineIdentityPointer->GetPlayerNickname(UserID));
			return;
		}
	}

	OnLoginCompleted.Broadcast(false, "");
}
