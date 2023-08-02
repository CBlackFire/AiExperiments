// <copyright file="SoulShardsGameInstance.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/GameInstance/ClientGameInstanceManager.h"
#include "GameMode/GameInstance/ServerGameInstanceManager.h"
#include "GameMode/GameInstance/ReplayManager.h"
#include "GameMode/GameInstance/TournamentManager.h"
#include "GameMode/Data/MapContainer.h"
#include "GameMode/Data/DeckContainer.h"
#include "GameMode/Data/CameraScriptContainer.h"
#include "GameMode/Data/DynamicAssetContainer.h"
#include "BattleEngine/D20/SoulCasting/CreatureSelector.h"
#include "AutoSettings/Public/SettingsManager.h"
#include "AutoSettingsInput/Public/InputMappingManager.h"
#include "AutoSettingsInput/Public/GlobalKeyIconTagManager.h"
#include "SoulShardsGameInstance.generated.h"

UCLASS(config = Game)
/// <summary>
/// Game Instance that the player is connected to
/// </summary>
/// <seealso cref="UGameInstance" />
class SOULSHARDS_API USoulShardsGameInstance : public UGameInstance
{
	GENERATED_BODY()
private:

	//UPROPERTY(transient)
	/// <summary>
	/// AutoSettings settings manager singleton.
	/// </summary>
	//USettingsManager* settingsManager;

	//UPROPERTY(transient)
	/// <summary>
	/// AutoSettings input manager singleton.
	/// </summary>
	//UInputMappingManager* inputMappingManager;

	//UPROPERTY(transient)
	/// <summary>
	/// AutoSettings cvar change listener manager singleton.
	/// </summary>
	//UCVarChangeListenerManager* cvarChangeListenerManager;

	//UPROPERTY(transient)
	/// <summary>
	/// AutoSettings global key icon manager singleton.
	/// </summary>
	//UGlobalKeyIconTagManager* globalKeyIconTagManager;

	UPROPERTY(transient)
	/// <summary>
	/// The client game instance manager.
	/// </summary>
	UClientGameInstanceManager* ClientGameInstanceManager;

	UPROPERTY(transient)
	/// <summary>
	/// The client game instance manager.
	/// </summary>
	UReplayManager* ReplayManager;

	UPROPERTY(transient)
	/// <summary>
	/// The client game instance tournament manager.
	/// </summary>
	UTournamentManager* TournamentManager;

	UPROPERTY(transient)
	/// <summary>
	/// The server game instance manager.
	/// </summary>
	UServerGameInstanceManager* ServerGameInstanceManager;

	UPROPERTY(transient)
	/// <summary>
	/// The game instance manager currently used to connect to a session.  This is the serverGameInstanceManager in case we are in a dedicated server.
	/// If we are in a client this is clientGameInstanceManager in case the player is connected to a server, and serverGameInstanceManager in case
	/// the player is playing offline.
	/// </summary>
	UBaseGameInstanceManager* CurrentGameInstanceManager;
	
	UPROPERTY(transient)
	/// <summary>
	/// The dynamic spell asset container
	/// </summary>
	UDynamicAssetContainer* DynamicSpellAssetContainer;

	UPROPERTY(transient)
	/// <summary>
	/// The dynamic caster asset container
	/// </summary>
	UDynamicAssetContainer* DynamicCasterAssetContainer;

	UPROPERTY(transient)
	/// <summary>
	/// The dynamic material asset container
	/// </summary>
	UDynamicAssetContainer* DynamicMaterialAssetContainer;

	UPROPERTY(transient)
	/// <summary>
	/// The scripted level asset container
	/// </summary>
	UDynamicAssetContainer* ScriptedLevelAssetContainer;

	UPROPERTY(transient)
	/// <summary>
	/// The misc asset container
	/// </summary>
	UDynamicAssetContainer* MiscAssetContainer;

	/// <summary>
	/// The user custom id
	/// </summary>
	FString CustomUserId;

	/// <summary>
	/// The command line the application started with
	/// </summary>
	FString commandLine;

	UPROPERTY(transient)
	/// <summary>
	/// The map container instance
	/// </summary>
	UMapContainer* MapContainer;

	UPROPERTY(transient)
	/// <summary>
	/// The map container instance
	/// </summary>
	UDeckContainer* DeckContainer;

	UPROPERTY(transient)
	/// <summary>
	/// The camera script container instance
	/// </summary>
	UCameraScriptContainer* CameraScriptContainer;

	UPROPERTY(transient)
	/// <summary>
	/// The widget container instance
	/// </summary>
	UCameraScriptContainer* WidgetContainer;

	UPROPERTY(transient)
	/// <summary>
	/// Class that contains the dark void spell manifest
	/// </summary>
	TSubclassOf<UDynamicAssetContainer> DarkVoidSpellManifestClass;

	UPROPERTY(transient)
	/// <summary>
	/// Class that contains the ice void spell manifest
	/// </summary>
	TSubclassOf<UDynamicAssetContainer> IceVoidSpellManifestClass;

	UPROPERTY(transient)
	/// <summary>
	/// Class that contains the no realm spell manifest
	/// </summary>
	TSubclassOf<UDynamicAssetContainer> NoRealmSpellManifestClass;

	UPROPERTY(transient)
	/// <summary>
	/// Class that contains the world enchant spell manifest
	/// </summary>
	TSubclassOf<UDynamicAssetContainer> WorldEnchantSpellManifestClass;

	UPROPERTY(transient)
	/// <summary>
	/// Class that contains the scripted level manifest
	/// </summary>
	TSubclassOf<UDynamicAssetContainer> ScriptedLevelManifestClass;

	UPROPERTY(transient)
	/// <summary>
	/// Class that contains the material manifest
	/// </summary>
	TSubclassOf<UDynamicAssetContainer> MaterialManifestClass;

	UPROPERTY(transient)
	/// <summary>
	/// Class that contains the caster manifest
	/// </summary>
	TSubclassOf<UDynamicAssetContainer> CasterManifestClass;

	UPROPERTY(transient)
	/// <summary>
	/// Class that contains the map container manifest
	/// </summary>
	TSubclassOf<UMapContainer> MapContainerClass;

	UPROPERTY(transient)
	/// <summary>
	/// Class that contains the deck container manifest
	/// </summary>
	TSubclassOf< UDeckContainer> DeckContainerClass;

	UPROPERTY(transient)
	/// <summary>
	/// Class that contains the camera script container manifest
	/// </summary>
	TSubclassOf<UCameraScriptContainer> CameraScriptContainerClass;

	UPROPERTY(transient)
	/// <summary>
	/// Class that contains the scripted level manifest
	/// </summary>
	TSubclassOf<UDynamicAssetContainer> MiscManifestClass;

protected:

	/// <summary>
	/// Called when the instance is shut down (so cleanup can be done).
	/// </summary>
	virtual void Shutdown() override;

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="USoulShardsGameInstance"/> class.
	/// </summary>
	USoulShardsGameInstance();

	UFUNCTION(BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Moves a file in the file system
	/// </summary>
	/// <param name="filePath">The path of the file to be moved.</param>
	/// <param name="newPath">The new path of the file.</param>
	static bool MoveFile(FString filePath, FString newPath);

	UFUNCTION(BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Moves a file in the file system
	/// </summary>
	/// <param name="filePath">The path of the file to be copied.</param>
	/// <param name="newPath">The new path of the file.</param>
	static void CopyFile(FString filePath, FString newPath);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = USoulShardsGameInstance)
	/// <summary>
	/// Dynamically gets a class by its name
	/// </summary>
	/// <param name="classLocation">Location of the class to be retrieved.</param>
	static UClass* GetClassByString(FString classPath, bool checkResult = false);

	UFUNCTION(BlueprintPure, Category = USoulShardsGameInstance)
	/// <summary>
	/// Determines whether the current buid is editor enabled.
	/// </summary>
	static bool IsWithEditor()
	{
#if WITH_EDITOR
		return true;
#else
		return false;
#endif
	};

	UFUNCTION(BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Destroys the session and leave game.
	/// </summary>
	/// <param name="submitMatchResults">Value indicating whether submit match results.</param>
	void LeaveGame(bool submitMatchResults = true);

	UFUNCTION()
	/// <summary>
	/// Destroys the session, leave game and submit match results.
	/// </summary>
	void LeaveGameAndSubmitMatchResults();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Gets the dynamic spell asset container
	/// </summary>
	UDynamicAssetContainer* GetDynamicSpellAssetContainer();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Gets the dynamic caster asset container
	/// </summary>
	UDynamicAssetContainer* GetDynamicCasterAssetContainer();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Gets the dynamic material asset container
	/// </summary>
	UDynamicAssetContainer* GetDynamicMaterialAssetContainer();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Gets the scripted level asset container
	/// </summary>
	UDynamicAssetContainer* GetScriptedLevelAssetContainer();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Gets the mic asset container
	/// </summary>
	UDynamicAssetContainer* GetMiscAssetContainer();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Gets the client game instance manager.
	/// </summary>
	UClientGameInstanceManager* GetClientGameInstanceManager();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Gets the replay manager.
	/// </summary>
	UReplayManager* GetReplayManager();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Gets the tournament manager.
	/// </summary>
	UTournamentManager* GetTournamentManager();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Gets the server game instance manager.
	/// </summary>
	UServerGameInstanceManager* GetServerGameInstanceManager();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Gets the current game instance manager.
	/// </summary>
	/// <returns></returns>
	UBaseGameInstanceManager* GetCurrentGameInstanceManager();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Returns true if the current game is queued for multiplayer
	/// </summary>
	/// <returns></returns>
	bool GetIsQueuedForMultiplayer();

	/// <summary>
	/// Gets the custom user id.
	/// </summary>
	/// <returns></returns>
	FString GetCusmtomUserId();

	/// <summary>
	/// Sets the custom user id.
	/// </summary>
	/// <param name="customUserId">The custom user id.</param>
	void SetCusmtomUserId(FString customUserId);

	/// <summary>
	/// Sets the current game instance manager.
	/// </summary>
	/// <param name="currentGameInstanceManager">The current game instance manager.</param>
	void SetCurrentGameInstanceManager(UBaseGameInstanceManager* currentGameInstanceManager);

	UFUNCTION(BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Gets a flag that indicates whether this instance is running on a dedicated server.
	/// </summary>
	bool GetIsDedicatedServer();

	UFUNCTION(BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Gets the stored command line (that the process started with) and clears the value.
	/// </summary>
	FString PopCommandLine();

	/// <summary>
	/// Gets the map container which can be used to get all playable arenas and pick random ones.
	/// </summary>
	UMapContainer* GetMapContiner();

	UFUNCTION(BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Gets the deck container which can be used to get predefined local decks.
	/// </summary>
	UDeckContainer* GetDeckContiner();	

	UFUNCTION(BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Gets the camera script container which can be used to get predefined local camera scripts.
	/// </summary>
	UCameraScriptContainer* GetCameraScriptContainer();

	/// <summary>
	/// Gets map data for a game type
	/// </summary>
	/// <param name="gameType">Type of the game.</param>
	/// <param name="gameRealm">The game realm.</param>
	/// <returns></returns>
	FMapData GetMapForGameType(EGameType gameType, FStringAssetReference specificMap, EGameRealm gameRealm = EGameRealm::EGRUknown);

	/// <summary>
	/// Cleans up the game instance
	/// </summary>
	virtual void FinishDestroy() override;
};
