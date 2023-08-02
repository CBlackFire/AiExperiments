// <copyright file="ServerGameInstanceManager.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/GameInstance/BaseGameInstanceManager.h"
#include "GameMode/Data/HttpRequestQueueData.h"
#include "GameMode/Data/MapData.h"
#include "Player/Data/PlayerData.h"
#include "ServerGameInstanceManager.generated.h"

UCLASS(Config = Game)
/// <summary>
/// Responsible for handling specific aspects of server game instances.
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API UServerGameInstanceManager : public UBaseGameInstanceManager
{
	GENERATED_BODY()
	
private:

	UPROPERTY(Config)
	/// <summary>
	/// The time in seconds that should pass before the game drops because multiplayer match not having started.
	/// </summary>
	float MultiplayerMatchNotStartedTimeBeforeDropping;

	UPROPERTY(Config)
	/// <summary>
	/// The time in seconds that should pass before the game drops because there are no players logged in
	/// </summary>
	float NoPlayersTimeBeforeDropping;

	UPROPERTY(Config)
	/// <summary>
	/// Indicates whether to use the Playfab GDK
	/// </summary>
	bool UsePlayfabGsdk;

	UPROPERTY(Config)
	/// <summary>
	/// The delay time in seconds before submit match retry.
	/// </summary>
	float DelayInSecondsBeforeSubmitMatchRetry;

	UPROPERTY(Config)
	/// <summary>
	/// The max submit match tries
	/// </summary>
	int MaxSubmitMatchTries;

	/// <summary>
	/// Flag indicating whether the multiplayer match has started.
	/// </summary>
	bool hasMultiplayerMatchStarted;

	/// <summary>
	/// The information for the players before log in is finished.  This helps move options information from pre-login to post-login
	/// </summary>
	TMap<FString, FPlayerData> preLoggedInPlayers;

	/// <summary>
	/// The logged in players
	/// </summary>
	TMap<FString, FPlayerData> loggedInPlayers;

	/// <summary>
	/// The current game type
	/// </summary>
	EGameType currentGameType;

	/// <summary>
	/// The current game identifier
	/// </summary>
	FGuid currentGameId;

	/// <summary>
	/// State indicating whether the dedicated server is shutting down
	/// </summary>
	EServerShutDownState shutDownState;

	/// <summary>
	/// The http request queue.
	/// </summary>
	TArray<FHttpRequestQueueData> submitMatchResultsRequestQueue;

	/// <summary>
	/// Map that this server instance will use.
	/// </summary>
	FMapData selectedMap;

	/// <summary>
	/// Indicates whether the selected map has been preloaded.
	/// </summary>
	bool isSelectedMapLoaded;

	/// <summary>
	/// Callback invoked by playfab when it wants to shut down the game.
	/// </summary>
	static void OnPlayFabShutdown();

	/// <summary>
	/// Callback invoked by playfab to figure out if the server is healthy.
	/// </summary>
	static bool IsHealthy();	

	/// <summary>
	/// Called to start the online game once the players are ready (Perform server travel)
	/// </summary>
	void StartOnlineGame();

	/// <summary>
	/// Gets the travel path for current game type.  This is the travel path to the actual game screen
	/// </summary>
	/// <param name="mapPath">The map path.</param>
	/// <returns></returns>
	FString GetServerTravelPath(FString mapPath);

	UFUNCTION()
	/// <summary>
	/// Quits the game if there are no players
	/// </summary>
	void QuitIfNoPlayers();

	UFUNCTION()
	/// <summary>
	/// Quits the game if the multiplayer match has not been started
	/// </summary>
	void QuitIfMultiplayerMatchNotStarted();

	/// <summary>
	/// Finds a pre-logged-in player by matching username string with the start of the stored usernames.
	/// This was necessary because the
	/// pre-logged in user names may be longer.
	/// </summary>
	FPlayerData FindPreLoggedInPlayer(const FString& userName) const;

	UFUNCTION()
	/// <summary>
	/// Called when http completes a call
	/// </summary>
	void OnGameClientHttpCompleted(bool success);

	UFUNCTION()
	/// <summary>
	/// Called when http fails to complete a call
	/// </summary>
	/// <param name="response">The response.</param>
	void OnGameClientHttpFailure(EHttpFailureReason failureReason);

protected:

	/// <summary>
	/// Ticks this instance.
	/// </summary>
	virtual void Tick() override;

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="UServerGameInstanceManager"/> class.
	/// </summary>
	UServerGameInstanceManager();

	/// <summary>
	/// Hosts a session of the specified game type
	/// </summary>
	/// <param name="gameType">GameType.</param>
	/// <param name="gameId">The game identifier.</param>
	/// <returns></returns>
	bool HostSession(EGameType gameType, FGuid gameId);

	/// <summary>
	/// Called before a player logs in into a dedicated server.
	/// </summary>
	/// <param name="userName">The user name.</param>
	/// <param name="gameOptions">The game options.</param>
	/// <returns>
	/// An error message if the user is denied
	/// </returns>
	FString DedicatedServerApproveLogin(FString userName, FGameOptionsData gameOptions);

	/// <summary>
	/// Called after a player logs in into a dedicated server.
	/// </summary>
	/// <param name="newPlayer">The new player.</param>
	void DedicatedServerPostLogin(const APlayerController* newPlayer);

	/// <summary>
	/// Called when a player logs out of a dedicated server.
	/// </summary>
	/// <param name="controller">The controller.</param>
	void DedicatedServerLogout(const APlayerController* controller);

	UFUNCTION()
	/// <summary>
	/// Performs a server a server travel using this instance's information
	/// </summary>
	void PerformServerTravel(EGameType gameTye);

	/// <summary>
	/// Gets the logged in players
	/// </summary>
	TMap<FString, FPlayerData> GetLoggedInPlayers();

	/// <summary>
	/// Logs in for a local game with a selected deck
	/// </summary>
	/// <param name="playerController">The player controller.</param>
	/// <param name="gameOptions">The game options.</param>
	void LocalLogin(const APlayerController* playerController, FGameOptionsData gameOptions);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UBaseGameInstanceManager)
	/// <summary>
	/// Gets the type of the current game.
	/// </summary>
	/// <returns></returns>
	EGameType GetCurrentGameType();

	/// <summary>
	/// Sets the type of the current game.
	/// </summary>
	/// <returns></returns>
	void SetCurrentGameType(EGameType gameType);

	/// <summary>
	/// Gets the id of the current game.
	/// </summary>
	/// <returns></returns>
	FGuid GetCurrentGameId() const;

	/// <summary>
	/// Sets the id of the current game.
	/// </summary>
	/// <returns></returns>
	void SetCurrentGameId(FGuid gameId);

	/// <summary>
	/// Gets a flag indicating whether the server is shutting down
	/// </summary>
	EServerShutDownState GetShutDownState() const;

	UFUNCTION()
	/// <summary>
	/// Quits the game after a critical error
	/// </summary>
	void ShutdownOnCriticalError();

	/// <summary>
	/// Quits the game after a critical error
	/// </summary>
	void ShutdownOnCriticalError(float timer);

	/// <summary>
	/// Indicates whether playfab Gdk is being used
	/// </summary>
	bool GetUsePlayfabGsdk() const;

	/// <summary>
	/// Adds an request to the current http request queue.
	/// </summary>
	/// <param name="url">The request url.</param>
	/// <param name="url">The request content.</param>
	void AddToHttpRequestQueue(FString url, FString content);

	/// <summary>
	/// Value indicating whether the http request queue is empty.
	/// </summary>
	bool IsHttpRequestQueueEmpty() const;
	
	/// <summary>
	/// Sets the parent game instance
	/// </summary>
	virtual void SetParentGameInstance(UGameInstance* gameInstance) override;

	/// <summary>
	/// Called by Game Mode when server travel finishes on dedicated server.
	/// </summary>
	void OnServerTravelFinished();
};
