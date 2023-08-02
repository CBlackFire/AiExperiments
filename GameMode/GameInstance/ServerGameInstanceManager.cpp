// <copyright file="ServerGameInstanceManager.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>


#include "SoulShards.h"
#include "GameMode/GameInstance/SoulShardsGameInstance.h"

#if UE_SERVER
#include "ThirdParty/PlayfabGSDK/include/gsdk.h"
#endif

#include "ServerGameInstanceManager.h"

#include "GameMode/BattleArenaGameMode.h"

UServerGameInstanceManager::UServerGameInstanceManager()
{
	this->currentGameType = EGameType::EGTAuto;

	this->shutDownState = EServerShutDownState::SSSRunning;
	this->hasMultiplayerMatchStarted = false;
	this->UsePlayfabGsdk = false;
	this->isSelectedMapLoaded = false;

	FNetworkVersion::IsNetworkCompatibleOverride.BindLambda([=](int, int)->bool { return true; });
}

void UServerGameInstanceManager::OnPlayFabShutdown()
{
	if (GEngine)
	{		
		GEngine->Exec(nullptr, TEXT("QUIT"));
	}
}

bool UServerGameInstanceManager::IsHealthy()
{
	return true;
}

bool UServerGameInstanceManager::HostSession(EGameType gameType, FGuid gameId)
{
	USoulShardsGameInstance* soulShardsGameInstance = static_cast<USoulShardsGameInstance*>(this->GetParentGameInstance());
	// We assume multiplayer is always for duel games here.  This is because we must select map types before knowing the type game,
	// in order to pre-load before game is frozen.
	this->SetCurrentGameType(gameType);
	this->selectedMap = soulShardsGameInstance->GetMapForGameType(gameType, FStringAssetReference());

	this->PerformServerTravel(EGameType::EGTDuelGame);	

	return true;
}

void UServerGameInstanceManager::LocalLogin(const APlayerController* playerController, FGameOptionsData gameOptions)
{
	this->SetCurrentGameInstanceManager();
	this->SetCurrentGameType(gameOptions.GameType);
	FPlayerData newPlayer;
	 
	newPlayer.UserName = playerController->PlayerState->GetPlayerName();
	newPlayer.FriendlyUserName = SoulShardsBaseHelper::GetFriendlyTitleDisplayName(newPlayer.UserName);
	
	newPlayer.GameOptions = gameOptions;
	this->loggedInPlayers.Reset();
	this->loggedInPlayers.Add(newPlayer.UserName, newPlayer);

	// For local games we don't preload maps
	this->isSelectedMapLoaded = true;
	this->selectedMap.MapAsset = nullptr;
}

void UServerGameInstanceManager::StartOnlineGame()
{
	UE_LOG(SoulShardsLog, Log, TEXT("UServerGameInstanceManager::StartOnlineGame() invoked."));

	TArray<FPlayerData> players;
	this->loggedInPlayers.GenerateValueArray(players);
	for (auto loggedInPlayer: players)
	{
		const auto gameMode = this->GetWorld()->GetAuthGameMode<ABattleArenaGameMode>();
		if (gameMode != nullptr)
		{
			gameMode->OnPlayerJoined(loggedInPlayer.PlayerControllerActor);
		}
	}
	
	this->hasMultiplayerMatchStarted = true;	
}

FString UServerGameInstanceManager::GetServerTravelPath(FString mapPath)
{
	const FString gameModeOptions = this->GetGameOptionsForGameType(this->GetCurrentGameType());
	FString mapUrl = mapPath + gameModeOptions;
	return mapUrl;
}

FString UServerGameInstanceManager::DedicatedServerApproveLogin(FString userName, FGameOptionsData gameOptions)
{
	UE_LOG(
		SoulShardsLog,
		Log,
		TEXT("UServerGameInstanceManager::DedicatedServerApproveLogin() started, userName = %s, gameType = %i"),
		*userName,
		static_cast<int>(gameOptions.GameType));

	FPlayerData tmpPlayer;
	// Unreal internally uses only the first 16 characters for the name later, so we match this in order for approve login and post login
	// to match names
	tmpPlayer.UserName = userName;
	tmpPlayer.GameOptions = gameOptions;

	if (this->loggedInPlayers.Contains(tmpPlayer.UserName))
	{
		return FString(TEXT("Same player trying to login twice into the same session"));
	}

	const EGameType currentGameType = this->GetCurrentGameType();
	if (currentGameType != EGameType::EGTAuto)
	{
		if (currentGameType != gameOptions.GameType)
		{
			return FString(TEXT("Trying to join a game of different type"));
		}

		int maxPlayers = 1;
		if (this->GetIsMultiplayerGameType(currentGameType))
		{
			maxPlayers = 2;
		}

		if (this->loggedInPlayers.Num() == maxPlayers)
		{
			return FString(TEXT("Game already has all players it can host"));
		}
	}

	if (this->GetCurrentGameType() == EGameType::EGTAuto)
	{
		UE_LOG(
			SoulShardsLog,
			Log,
			TEXT("UServerGameInstanceManager::DedicatedServerApproveLogin() transitioning from Auto game type to %i"),
			static_cast<int>(gameOptions.GameType));

		this->SetCurrentGameType(gameOptions.GameType);
		this->SetCurrentGameId(gameOptions.GameId);

	}

	if (!this->preLoggedInPlayers.Contains(tmpPlayer.UserName))
	{
		this->preLoggedInPlayers.Add(tmpPlayer.UserName, tmpPlayer);
	}

	return FString();
}

TMap<FString, FPlayerData> UServerGameInstanceManager::GetLoggedInPlayers()
{
	return this->loggedInPlayers;
}

FPlayerData UServerGameInstanceManager::FindPreLoggedInPlayer(const FString& userName) const
{	
	TArray<FPlayerData> playersData;
	this->preLoggedInPlayers.GenerateValueArray(playersData);
	for (auto& playerData : playersData)
	{
		if (playerData.UserName.StartsWith(userName))
		{
			return playerData;
		}
	}

	FPlayerData empty;
	return empty;
}

void UServerGameInstanceManager::DedicatedServerPostLogin(const APlayerController* newPlayer)
{
	FString userName = newPlayer->PlayerState->GetPlayerName();
	UE_LOG(SoulShardsLog, Log, TEXT("UServerGameInstanceManager::DedicatedServerPostLogin() started, userName = %s"), *userName);

	FPlayerData tmpPlayer;

	tmpPlayer.UserName = userName;
	tmpPlayer.PlayerControllerActor = const_cast<APlayerController*>(newPlayer);
	FString friendlyUserName = SoulShardsBaseHelper::GetFriendlyTitleDisplayName(tmpPlayer.UserName);

	const FPlayerData preLoggedInPlayer = this->FindPreLoggedInPlayer(userName);

	if (!preLoggedInPlayer.UserName.IsEmpty())
	{
		// The player options are copied if found
		tmpPlayer.GameOptions = preLoggedInPlayer.GameOptions;
		UE_LOG(SoulShardsLog, Log, TEXT("UServerGameInstanceManager::DedicatedServerPostLogin() copying options: userName = %s, deckId = %s"), *userName, *tmpPlayer.GameOptions.DeckId);

		this->loggedInPlayers.Add(tmpPlayer.UserName, tmpPlayer);

		if (this->loggedInPlayers.Num() == 2)
		{
			this->StartOnlineGame();
		}
	}
}

void UServerGameInstanceManager::DedicatedServerLogout(const APlayerController* controller)
{
	FString playerName = controller->PlayerState->GetPlayerName();

	UE_LOG(SoulShardsLog, Log, TEXT("UServerGameInstanceManager::DedicatedServerLogout() player %s disconnected"), *playerName);

	FPlayerData tmpPlayer;
	tmpPlayer.UserName = playerName;

	this->loggedInPlayers.Remove(tmpPlayer.UserName);
	this->preLoggedInPlayers.Remove(tmpPlayer.UserName);

	// If there are no more players in the game, we will wait some time to see if players reconnect.
	// If they don't we kill the game.
	if (this->loggedInPlayers.Num() == 0)
	{
		UE_LOG(SoulShardsLog, Log, TEXT("UServerGameInstanceManager::DedicatedServerLogout() no more players.  Game will end in %f seconds if no more players connect"), this->NoPlayersTimeBeforeDropping);
		FTimerHandle UnusedHandle;
		this->GetWorld()->GetTimerManager().SetTimer(
			UnusedHandle,
			this,
			&UServerGameInstanceManager::QuitIfNoPlayers,
			this->NoPlayersTimeBeforeDropping,
			false);
	}
}

void UServerGameInstanceManager::ShutdownOnCriticalError()
{
	UE_LOG(SoulShardsLog, Log, TEXT("UServerGameInstanceManager::ShutdownOnTimer().  Shutting down due to critical error..."));
	this->LeaveGame();
}

void UServerGameInstanceManager::ShutdownOnCriticalError(float timer)
{
	this->shutDownState = EServerShutDownState::SSSShutDownScheduled;
	FTimerHandle unusedHandle;
	this->GetWorld()->GetTimerManager().SetTimer(
		unusedHandle,
		this,
		&UServerGameInstanceManager::ShutdownOnCriticalError,
		15.0f,
		false);
}

void UServerGameInstanceManager::QuitIfNoPlayers()
{
	UE_LOG(SoulShardsLog, Log, TEXT("UServerGameInstanceManager::QuitIfNoPlayers()"));
	if (this->loggedInPlayers.Num() == 0)
	{
		UE_LOG(SoulShardsLog, Log, TEXT("UServerGameInstanceManager::QuitIfNoPlayers() no more players after wait.  Exiting game"));
		this->shutDownState = EServerShutDownState::SSSShutDownExecuted;
		this->LeaveGame();
	}
	else
	{
		this->shutDownState = EServerShutDownState::SSSRunning;
	}
}

void UServerGameInstanceManager::QuitIfMultiplayerMatchNotStarted()
{
	UE_LOG(SoulShardsLog, Log, TEXT("UServerGameInstanceManager::QuitIfMultiplayerMatchNotStarted()"));
	if (!this->hasMultiplayerMatchStarted)
	{
		UE_LOG(SoulShardsLog, Log, TEXT("UServerGameInstanceManager::QuitIfMultiplayerMatchNotStarted() Online game not started in allocated time.  Exiting game"));
		this->shutDownState = EServerShutDownState::SSSShutDownExecuted;
		this->LeaveGame();
	}
	else
	{
		this->shutDownState = EServerShutDownState::SSSRunning;
	}
}

void UServerGameInstanceManager::PerformServerTravel(EGameType gameType)
{
	// Multiplayer travel
	UE_LOG(SoulShardsLog, Log, TEXT("UServerGameInstanceManager::PerformServerTravel() opening level: %s"), *this->selectedMap.GetMapAssetName());
	if (this->selectedMap.MapAsset.IsNull())
	{
		USoulShardsGameInstance* soulShardsGameInstance = static_cast<USoulShardsGameInstance*>(this->GetParentGameInstance());
		this->selectedMap = soulShardsGameInstance->GetMapForGameType(gameType, FStringAssetReference());
	}
	
	const FString serverTravelPath = this->GetServerTravelPath(this->selectedMap.GetMapAssetName());
	//TODO: update session
	//this->GetWorld()->ServerTravel(serverTravelPath, true);
	this->GetWorld()->SeamlessTravel(serverTravelPath, true);
	
}

EGameType UServerGameInstanceManager::GetCurrentGameType()
{
	return this->currentGameType;
}

void UServerGameInstanceManager::SetCurrentGameType(EGameType gameType)
{

	this->currentGameType = gameType;
}

FGuid UServerGameInstanceManager::GetCurrentGameId() const
{
	return this->currentGameId;
}

void UServerGameInstanceManager::SetCurrentGameId(FGuid gameId)
{
	this->currentGameId = gameId;
}

EServerShutDownState UServerGameInstanceManager::GetShutDownState() const
{
	return this->shutDownState;
}

bool UServerGameInstanceManager::GetUsePlayfabGsdk() const
{
	return UsePlayfabGsdk;
}

void UServerGameInstanceManager::AddToHttpRequestQueue(FString url, FString content)
{
	const FHttpRequestQueueData httpRequestQueueData(url, content);
	this->submitMatchResultsRequestQueue.Add(httpRequestQueueData);
}

bool UServerGameInstanceManager::IsHttpRequestQueueEmpty() const
{
	return this->submitMatchResultsRequestQueue.Num() == 0;
}

void UServerGameInstanceManager::SetParentGameInstance(UGameInstance* gameInstance)
{
	Super::SetParentGameInstance(gameInstance);
}

void UServerGameInstanceManager::OnServerTravelFinished()
{
	UE_LOG(SoulShardsLog, Log, TEXT("UServerGameInstanceManager::OnServerTravelFinished() battle arena level pre-loaded."));
	
	this->isSelectedMapLoaded = true;

#if UE_SERVER
	
	if (this->UsePlayfabGsdk)
	{
		Microsoft::Azure::Gaming::GSDK::registerShutdownCallback(&UServerGameInstanceManager::OnPlayFabShutdown);
		Microsoft::Azure::Gaming::GSDK::registerHealthCallback(&UServerGameInstanceManager::IsHealthy);

		// Notify GSDK that the server is ready to start receiving players.
		// Call this when your game is done initializing and players can connect
		// Note: This is a blocking call, and will return when this game server is either allocated or terminated
		if (Microsoft::Azure::Gaming::GSDK::readyForPlayers())
		{

			UE_LOG(SoulShardsLog, Log, TEXT("UServerGameInstanceManager::HostSession() Setup QuitIfMultiplayerMatchNotStarted timer."));
			// We set a timer, we'll quit if match hasn't started before that time
			FTimerHandle UnusedHandle;
			this->GetWorld()->GetTimerManager().SetTimer(
				UnusedHandle,
				this,
				&UServerGameInstanceManager::QuitIfMultiplayerMatchNotStarted,
				this->MultiplayerMatchNotStartedTimeBeforeDropping,
				false);
		}
		else
		{
			UE_LOG(SoulShardsLog, Log, TEXT("UServerGameInstanceManager::HostSession() Playfab terminated server.  Exiting game"));
			this->shutDownState = EServerShutDownState::SSSShutDownExecuted;
			this->LeaveGame();
		}
	}

#endif
	
}

void UServerGameInstanceManager::Tick()
{
	if (this->isSelectedMapLoaded)
	{
		AHttpEnabledPlayerController* playerController = static_cast<AHttpEnabledPlayerController*>(this->GetParentGameInstance()->GetFirstLocalPlayerController());

		if ((playerController != nullptr) && (this->submitMatchResultsRequestQueue.Num() > 0))
		{
			FHttpRequestQueueData& httpRequestQueueData = this->submitMatchResultsRequestQueue[0];
			const float currentTime = this->GetWorld()->GetTimeSeconds();
			if (httpRequestQueueData.ValidateSend(currentTime, this->DelayInSecondsBeforeSubmitMatchRetry, this->MaxSubmitMatchTries))
			{
				FHttpCallCompleted onHttpCallCompleted;
				FHttpFailureEvent failureEvent;
				failureEvent.BindDynamic(this, &UServerGameInstanceManager::OnGameClientHttpFailure);

				onHttpCallCompleted.BindDynamic(this, &UServerGameInstanceManager::OnGameClientHttpCompleted);

				playerController->GetHttpCallManager()->HttpSubmitMatchResults(httpRequestQueueData.Url, httpRequestQueueData.Content, onHttpCallCompleted, failureEvent);
			}
			else if (httpRequestQueueData.NeedRemove)
			{
				this->submitMatchResultsRequestQueue.RemoveAt(0);
			}
		}
	}
}

void UServerGameInstanceManager::OnGameClientHttpCompleted(bool success)
{
	if (this->submitMatchResultsRequestQueue.Num() > 0)
	{
		this->submitMatchResultsRequestQueue[0].NeedRemove = true;
	}
}

void UServerGameInstanceManager::OnGameClientHttpFailure(EHttpFailureReason failureReason)
{
}