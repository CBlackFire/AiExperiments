// <copyright file="BaseGameInstanceManager.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsEnums.h"
#include "Player/Data/GameOptionsData.h"
#include "BaseGameInstanceManager.generated.h"

UCLASS(config = Game, BlueprintType)
/// <summary>
/// Base class for Client and Server Game Instance managers.  These managers are responsible for handling specific aspects of client and server game instances.
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API UBaseGameInstanceManager : public UObject
{
	GENERATED_BODY()
	
private:

	/// <summary>
	/// The parent game instance
	/// </summary>
	UGameInstance* parentGameInstance;

	/// <summary>
	/// Name of the current session
	/// </summary>
	FName currentSessionName;
	
	UPROPERTY(Config)
	/// <summary>
	/// Name of the ranked multiplayer queue name.
	/// </summary>
	FString RanekdMultiplayerQueueName;

	UPROPERTY(Config)
	/// <summary>
	/// Name of the multiplayer queue name.
	/// </summary>
	FString MultiplayerQueueName;

#pragma region Session Management Fields

	/// <summary>
	/// Session settings
	/// </summary>
	TSharedPtr<class FOnlineSessionSettings> sessionSettings;
	

#pragma endregion

protected:

	/// <summary>
	/// Sets the name of the current session
	/// </summary>
	void SetCurrentSessionName(FName sessionName);


	/// <summary>
	/// Gets the session settings.
	/// </summary>
	/// <returns></returns>
	TSharedPtr<class FOnlineSessionSettings> GetSessionSettings();

	/// <summary>
	/// Sets the session settings.
	/// </summary>
	/// <param name="settings">The settings.</param>
	void SetSessionSettings(TSharedPtr<class FOnlineSessionSettings> settings);

	/// <summary>
	/// Sets the current game instance manager for the game instance to this manager
	/// </summary>
	/// <param name="currentGameInstanceManager">The current game instance manager.</param>
	void SetCurrentGameInstanceManager();

	/// <summary>
	/// Ticks this instance.  Custom tick implementation just for instance managers, based on timers
	/// </summary>
	virtual void Tick();

public:

	/// <summary>
	/// Gets the type of the game mode for game.
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>
	static FString GetGameModeForGameType(EGameType gameType);

	/// <summary>
	/// Initializes a new instance of the <see cref="UBaseGameInstanceManager"/> class.
	/// </summary>
	UBaseGameInstanceManager();

	/// <summary>
	/// Gets the world
	/// </summary>
	virtual UWorld* GetWorld() const override;


	/// <summary>
	/// Gets the name of the map.  This is a virtual name indicating game type and progress
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <param name="uniqueId">The unique identifier, in case the game requires one.</param>
	/// <param name="inProgress">flag indicating the in progress status of the game.</param>
	/// <returns></returns>
	FString GetMapName(EGameType gameType, FGuid uniqueId, bool inProgress);

#pragma region Game Options
	/// <summary>
	/// Gets the loby options for a game options data instance
	/// </summary>
	/// <param name="gameOptionsData">Game options.</param>
	/// <returns></returns>
	static FString GetLobbyOptionsForGameType(FGameOptionsData gameOptionsData);

	/// <summary>
	/// Gets the game options from string.
	/// </summary>
	/// <param name="options">The options string.</param>
	/// <returns></returns>
	static FGameOptionsData GetGameOptionsFromString(FString options);

#pragma endregion

#pragma region GameType methods
	/// <summary>
	/// Gets the type of the game mode for game.
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>
	static FString GetGameOptionsForGameType(EGameType gameType);	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UBaseGameInstanceManager)
	/// <summary>
	/// Determines whether a game type is online or offline
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>
	static bool GetIsOnlineGameType(EGameType gameType);

	/// <summary>
	/// Determines whether a game type is multiplayer or single player
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UBaseGameInstanceManager)
	static bool GetIsMultiplayerGameType(EGameType gameType);

	/// <summary>
	/// Determines whether the game type uses a unique identifier.
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>
	static bool GetGameTypeUsesUniqueId(EGameType gameType);

	UFUNCTION(BlueprintPure, BluePrintCallable, Category = UBaseGameInstanceManager)
	/// <summary>
	/// Gets a matching queued game type for the game type passed as parameter.
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>
	static EGameType GetMatchingQueuedGameType(EGameType gameType);

	UFUNCTION(BlueprintPure, BluePrintCallable, Category = UBaseGameInstanceManager)
	/// <summary>
	/// Gets a matching non queued game type for the game type passed as parameter.
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>
	static EGameType GetMatchingNonQueuedGameType(EGameType gameType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UBaseGameInstanceManager)
	/// <summary>
	/// Gets a boolean determining whether this is a queued game type.
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>
	static bool GetIsQueuedGameType(EGameType gameType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UBaseGameInstanceManager)
	/// <summary>
	/// Gets a boolean determining whether this game types requires a content id
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>
	static bool GetGameTypeUsesContentId(EGameType gameType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UBaseGameInstanceManager)
	/// <summary>
	/// Gets a boolean determining whether this game types requires a level selection
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>
	static bool GetGameTypeUsesLevel(EGameType gameType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UBaseGameInstanceManager)
	/// <summary>
	/// Gets a boolean determining whether this game types requires a realm selection
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>
	static bool GetGameTypeUsesRealm(EGameType gameType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UBaseGameInstanceManager)
	/// <summary>
	/// Gets a boolean determining whether this is a nightmare game type.
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>
	static bool GetIsNightmareGameType(EGameType gameType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UBaseGameInstanceManager)
	/// <summary>
	/// Gets a boolean determining whether this is a home defense game type.
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>
	static bool GetIsHomeDefenseType(EGameType gameType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UBaseGameInstanceManager)
	/// <summary>
	/// Determines whether a game type supports a score widget
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>	
	static bool GetSupportsScoreWidget(EGameType gameType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = UBaseGameInstanceManager)
	/// <summary>
	/// Determines the queue name for a given game type
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <returns></returns>	
	 FString GetQueueNameForGameType(EGameType gameType);

#pragma endregion

	/// <summary>
	/// Gets the name of the current session
	/// </summary>
	FName GetCurrentSessionName();

	/// <summary>
	/// Leaves the game.
	/// <param name="submitMatchResults">Value indicating whether submit match results.</param>
	/// </summary>
	void LeaveGame(bool submitMatchResults = true);

	/// <summary>
	/// Gets the parent game instance
	/// </summary>
	UGameInstance* GetParentGameInstance();

	/// <summary>
	/// Sets the parent game instance
	/// </summary>
	virtual void SetParentGameInstance(UGameInstance* gameInstance);
};
