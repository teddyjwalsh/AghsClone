// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SessionMenuInterface.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MainMenu.h"
#include "AghsCloneGameInstance.generated.h"

#define SERVER_NAME_SETTINGS_KEY "ServerName"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()
		FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUsername;
};

const static FName SESSION_NAME = TEXT("CoopPuzzleGameSession");

/**
 * 
 */
UCLASS()
class AGHSCLONE_API UAghsCloneGameInstance : public UGameInstance,
	public ISessionMenuInterface
{
	GENERATED_BODY()

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
	UMainMenu* MainMenu;

	void Init()
	{
		IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get();
		SessionInterface = SubSystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{

			// Subscribe to minimun events to handling sessions﻿﻿﻿﻿
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UAghsCloneGameInstance::OnCreateSessionComplete);
			
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UAghsCloneGameInstance::OnDestroySessionComplete);

			SessionInterface->OnFindSessionsCompleteDelegates.
				AddUObject(this, &UAghsCloneGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UAghsCloneGameInstance::OnJoinSessionsComplete);
		}
	}

	void OnCreateSessionComplete(FName name, bool result)
	{

	}

	void OnDestroySessionComplete(FName name, bool result)
	{

	}

	void OnJoinSessionsComplete(FName name, EOnJoinSessionCompleteResult::Type result)
	{
		// Get the url for that session f﻿irst
		FString Url;
		if (!SessionInterface->GetResolvedConnectString(SESSION_NAME, Url))
		{
			return;
		}
		// The player controller travels to that url on that specific 
		// session
		APlayerController* PlayerController =
			GetFirstLocalPlayerController();
		PlayerController->ClientTravel(Url,
			ETravelType::TRAVEL_Absolute);
	}

	void OnFindSessionsComplete(bool Success)
	{
		if (Success && SessionSearch.IsValid())
		{
			if (SessionSearch->SearchResults.Num() > 0)
			{
				TArray<FServerData> ServerData;
				for (const FOnlineSessionSearchResult& SearchResult :
					SessionSearch->SearchResults)
				{
					FServerData Data;
					FString ServerName;
					if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
					{
						Data.Name = ServerName;
					}
					// Fill information for the menu
					Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
					Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
					Data.HostUsername = SearchResult.Session.OwningUserName;
					ServerData.Add(Data);
				}
				// Send the information back to the menu
				//MainMenu->InitializeSessionsList(ServerData);
			}
		}
	}

	virtual void Host(FString ServerName)
	{
		FString DesiredServerName("Shark");
		if (SessionInterface.IsValid())
		{
			FOnlineSessionSettings SessionSettings;
			SessionSettings.bIsLANMatch = false;
				SessionSettings.NumPublicConnections = 2;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.bUsesPresence = true;
			SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName,
				EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
			SessionInterface->CreateSession(0, "AghsCloneSession",
				SessionSettings);
		}
	}

	virtual void JoinSession(uint32 Index)
	{
		// Create the pointer
		SessionSearch = MakeShareable(new FOnlineSessionSearch());
		if (SessionSearch.IsValid())
		{
			// Set properties
			SessionSearch->MaxSearchResults = 100;
			SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true,
				EOnlineComparisonOp::Equals);
			SessionInterface->FindSessions(0,
				SessionSearch.ToSharedRef());
		}

		if (Index < (uint32)(SessionSearch->SearchResults.Num()))
		{
			SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
		}
	}

	virtual void EndSession()
	{

	}

	void OpenSessionListMenu()
	{
		// Create the pointer
		SessionSearch = MakeShareable(new FOnlineSessionSearch());
		if (SessionSearch.IsValid())
		{
			// Set properties
			SessionSearch->MaxSearchResults = 100;
			SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true,
					EOnlineComparisonOp::Equals);
			SessionInterface->FindSessions(0,
				SessionSearch.ToSharedRef());
		}
	}
};
