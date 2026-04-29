// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "CGameInstance.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLoginCompletedDelegate, bool /*bWasSuccessful*/, const FString& /*UserNickname*/)

/**
 * 
 */
UCLASS()
class UCGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	
	/************************************/
	/*				Session Server		*/
	/************************************/

	bool GetIsSessionServer() const;

	FString GetSessionNameStr() const;
	int GetSessionPort() const;
	FString GetSessionSearchId() const;

	FName GetSessionNameKey() const;
	FName GetSessionPortKey() const;
	FName GetSessionSearchIDKey() const;

	FString GetCommandLineArgAsString(const FName& ParamName) const;
	int GetCommandLineArgAsInt(const FName& ParamName) const;

	IOnlineSessionPtr GetSessionPtr() const;
	IOnlineIdentityPtr GetIdentityPtr() const;

	void PlayerJoined(const FUniqueNetIdRepl& UniqueId);
	void PlayerLeft(const FUniqueNetIdRepl& UniqueId);

private:
	TArray<FUniqueNetIdRepl> PlayerRecord;

	void CreateSession();

	FString ServerSessionName;
	int ServerPort;

	void SessionCreated(FName SessionName, bool bWasSuccessful);

	UPROPERTY(EditDefaultsOnly, Category = "Session Termination")
	float SessionTerminationWaitTime = 60.0f;

	FTimerHandle SessionTerminationTimerHandler;
	void StartSessionTermination();
	void CancelSessionTermination();
	void TerminateSession();
	void OnSessionEnded(FName SessionName, bool bWasSuccessful);

	/************************************/
	/*			Map						*/
	/************************************/

private:
	void LoadMapAndListen(TSoftObjectPtr<UWorld> Level);

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> GameMap;

	/************************************/
	/*			Client					*/
	/************************************/

public:
	bool IsClientLoggedIn() const;
	void ClientAuthPortalLogin();
	FString GetPlayerNickName() const;

	FOnLoginCompletedDelegate OnLoginCompleted;

	void RequestCreateNewSession();
	void TryFindAndJoinSession();

	FName GetCoordinatorURLKey() const;
	FString GetCoordinatorURL() const;
	FString GetDefaultCoordinatorURL() const;
	void StartFindingAndJoinSession(FGuid SessionSearchID=FGuid());

private:
	void ClientLoginCompleted(int LocalPlayerNumber, bool bWasSuccessful, const FUniqueNetId& UserID, const FString& ErrorMsg);

	void SessionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FGuid SessionSearchID);

	UPROPERTY(EditDefaultsOnly, Category = "Session Search")
	float SessionSearchInterval = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Session Search")
	float SessionSearchTimeoutDuration = 60.0f;

	// the setting for the search
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void FindAndJoinSession(FGuid SessionSearchID = FGuid());

	void FindSessionCompleted(bool bWasSuccessful);

	void JoinSessionWithSearchResult(const class FOnlineSessionSearchResult& SearchResult);

	void JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult, int Port);

	FString GetTestingURL();
	FName GetTestingURLKey();

	void ReplacePort(FString& OutURL, int NewPort);

	void StopSessionSearches();

	FTimerHandle FindSessionTimerHandle;
	FTimerHandle FindSessionTimeoutTimerHandle;
};
