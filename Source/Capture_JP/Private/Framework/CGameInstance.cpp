// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "HttpModule.h"
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

bool UCGameInstance::IsClientLoggedIn() const
{
	IOnlineIdentityPtr OnlineIdentityPointer = GetIdentityPtr();
	if (OnlineIdentityPointer)
	{
		return OnlineIdentityPointer->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	}

	return false;
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

FString UCGameInstance::GetPlayerNickName() const
{
	IOnlineIdentityPtr OnlineIdentityPointer = GetIdentityPtr();

	if (OnlineIdentityPointer)
	{
		return OnlineIdentityPointer->GetPlayerNickname(0);
	}

	return "Not Logged In!";
}

void UCGameInstance::RequestCreateNewSession()
{
	UE_LOG(LogTemp, Warning, TEXT("--------------------------------------------------"))
		UE_LOG(LogTemp, Warning, TEXT("Requesting to create new session"))
		UE_LOG(LogTemp, Warning, TEXT("--------------------------------------------------"))
		FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FGuid SessionSearchID = FGuid::NewGuid();
	FString CoordinatorURL = GetCoordinatorURL();

	FString URL = FString::Printf(TEXT("%s/Session"), *CoordinatorURL);
	Request->SetURL(URL);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable<FJsonObject>(new FJsonObject);
	JsonObject->SetStringField(GetSessionNameKey().ToString(), FGuid::NewGuid().ToString());
	JsonObject->SetStringField(GetSessionSearchIDKey().ToString(), SessionSearchID.ToString());

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	Request->OnProcessRequestComplete().BindUObject(this, &UCGameInstance::SessionCreationRequestCompleted, SessionSearchID);
	if (Request->ProcessRequest())
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Creation Request Failed Right Away"))
	}
}

void UCGameInstance::TryFindAndJoinSession()
{
	UE_LOG(LogTemp, Warning, TEXT("--------------------------------------------------"))
	UE_LOG(LogTemp, Warning, TEXT("Trying to find and join new session"))
	UE_LOG(LogTemp, Warning, TEXT("--------------------------------------------------"))
	StartFindingAndJoinSession();
}

FName UCGameInstance::GetCoordinatorURLKey() const
{
	return FName("COORDINATOR_URL");
}

FString UCGameInstance::GetCoordinatorURL() const
{
	FString CoordinatorURL = GetCommandLineArgAsString(GetCoordinatorURLKey());
	UE_LOG(LogTemp, Warning, TEXT("Found Command Line Arg URL: #%s#"), *CoordinatorURL)
	if (CoordinatorURL != "")
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Commandline Arguement Coordinator URL: %s"), *CoordinatorURL)
		return CoordinatorURL;
	}

	return GetDefaultCoordinatorURL();
}

FString UCGameInstance::GetDefaultCoordinatorURL() const
{
	FString CoordinatorURL = "";
	//DefaultGame.ini has this info, GGameIni is a pre defined string of that file
	GConfig->GetString(TEXT("Capture.Net"), TEXT("CoordinatorURL"), CoordinatorURL, GGameIni);
	UE_LOG(LogTemp, Warning, TEXT("Get Default Coordinator URL: %s"), *CoordinatorURL)
	return CoordinatorURL;
}

void UCGameInstance::StartFindingAndJoinSession(FGuid SessionSearchID)
{
	StopSessionSearches();

	GetWorld()->GetTimerManager().SetTimer(FindSessionTimerHandle, 
		FTimerDelegate::CreateUObject(this, &UCGameInstance::FindAndJoinSession, SessionSearchID),
		SessionSearchInterval,
		true, 0
	);

	GetWorld()->GetTimerManager().SetTimer(FindSessionTimeoutTimerHandle,
		this, &UCGameInstance::StopSessionSearches, SessionSearchTimeoutDuration
	);
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

void UCGameInstance::SessionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FGuid SessionSearchID)
{
	if (bWasSuccessful && Response.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Create Request Successful!"))
		int32 ResponseCode = Response->GetResponseCode();
		if (ResponseCode == 200)
		{
			FString ResponseString = Response->GetContentAsString();
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
			int32 Port = 0;
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				Port = JsonObject->GetIntegerField(*(GetSessionPortKey().ToString()));
				StartFindingAndJoinSession(SessionSearchID);
			}
		}
	}
}

void UCGameInstance::FindAndJoinSession(FGuid SessionSearchID)
{
	if (IOnlineSessionPtr SessionPtr = GetSessionPtr())
	{
		SessionSearch = MakeShareable(new FOnlineSessionSearch);
		if (SessionSearch.IsValid())
		{
			SessionSearch->bIsLanQuery = false;
			SessionSearch->MaxSearchResults = 1;
			if (SessionSearchID.IsValid())
			{
				SessionSearch->QuerySettings.Set(GetSessionSearchIDKey(), SessionSearchID.ToString(), EOnlineComparisonOp::Equals);
			}

			SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
			SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UCGameInstance::FindSessionCompleted);
			if (!SessionPtr->FindSessions(0, SessionSearch.ToSharedRef()))
			{
				UE_LOG(LogTemp, Warning, TEXT("Find Session Failed Right Away!!!!!"))
					SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
			}
		}
	}
}

void UCGameInstance::FindSessionCompleted(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		return;
	}

	if (SessionSearch->SearchResults.Num() == 0)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Found Session!!!!"))
	StopSessionSearches();
	JoinSessionWithSearchResult(SessionSearch->SearchResults[0]);
}

void UCGameInstance::JoinSessionWithSearchResult(const FOnlineSessionSearchResult& SearchResult)
{
	if (IOnlineSessionPtr SessionPtr = GetSessionPtr())
	{
		FString SessionName = "";
		SearchResult.Session.SessionSettings.Get<FString>(GetSessionNameKey(), SessionName);

		int64 Port = 0;
		const FOnlineSessionSetting* PortSetting = SearchResult.Session.SessionSettings.Settings.Find(GetSessionPortKey());
		PortSetting->Data.GetValue(Port);
		UE_LOG(LogTemp, Warning, TEXT("Trying to join session with name: %s, and port: %d !!!!"), *(SessionName), Port)

		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UCGameInstance::JoinSessionCompleted, (int)Port);
		if (!SessionPtr->JoinSession(0, FName(SessionName), SearchResult))
		{
			UE_LOG(LogTemp, Warning, TEXT("Join Session Failed Right Away!!!!"))
			SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
		}
	}
}

void UCGameInstance::JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult, int Port)
{
	IOnlineSessionPtr SessionPtr = GetSessionPtr();
	if (SessionPtr && JoinResult == EOnJoinSessionCompleteResult::Success)
	{
		StopSessionSearches();

		UE_LOG(LogTemp, Warning, TEXT("Join Session: %s successful! port: %d"), *(SessionName.ToString()), Port)

		FString TravelURL = "";
		SessionPtr->GetResolvedConnectString(SessionName, TravelURL);

		FString TestingURL = GetTestingURL();
		if (!TestingURL.IsEmpty())
		{
			TravelURL = TestingURL;
		}

		ReplacePort(TravelURL, Port);
		UE_LOG(LogTemp, Warning, TEXT("Traveling to: %s"), *(TravelURL))
		GetFirstLocalPlayerController(GetWorld())->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
	}
}

FString UCGameInstance::GetTestingURL()
{
	FString TestURL = GetCommandLineArgAsString(GetTestingURLKey());
	UE_LOG(LogTemp, Warning, TEXT("Get Test URL: %s"), *TestURL)
	return TestURL;
}

FName UCGameInstance::GetTestingURLKey()
{
	return FName("TESTING_URL");
}

void UCGameInstance::ReplacePort(FString& OutURL, int NewPort)
{
	FURL URL = FURL(nullptr, *OutURL, ETravelType::TRAVEL_Absolute);
	URL.Port = NewPort;
	OutURL = URL.ToString();
}

void UCGameInstance::StopSessionSearches()
{
	GetWorld()->GetTimerManager().ClearTimer(FindSessionTimeoutTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FindSessionTimerHandle);

	if (IOnlineSessionPtr SessionPtr = GetSessionPtr())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}
}
