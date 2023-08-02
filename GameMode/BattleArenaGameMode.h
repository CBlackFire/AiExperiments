// <copyright file="BattleArenaGameMode.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "Group/TListGroupTemplate.hpp"
#include "SoulShardsGameMode.h"
#include "BattleEngine/Data/AllegianceData.h"
#include "Interface/BattleArenaObjectInterface.h"
#include "BattleArenaGamePhase.h"
#include "Data/SpawnQueueData.h"
#include "Data/PhaseEventData.h"
#include "Player/Data/MatchResultData.h"
#include "Player/Data/PlayerData.h"
#include "Character/Component/CoolDownManagerComponent.h"
#include "AiEngine/AiEngine/TPlayerAiRule.hpp"
#include "AiEngine/AiEngine/IAIPlayer.hpp"
#include "BattleArenaGameMode.generated.h"


UCLASS(config = Game)
/// <summary>
/// Game mode that is based on a Battle Arena
/// </summary>
/// <seealso cref="TBattleArenaEntity" />
/// <seealso cref="ASoulShardsGameMode" />
class SOULSHARDS_API ABattleArenaGameMode : public ASoulShardsGameMode, public IBattleArenaObjectInterface
{
	GENERATED_BODY()

private:
	
	UPROPERTY(Config)
	/// <summary>
	/// The min distance of creatures to the enemy totem.
	/// </summary>
	float MinDistanceToTotem;

	/// <summary>
	/// Set that indicates which game level features are disabled (by default all are enbaled).
	/// </summary>
	TSet<EConfigurableGameLevelFeature> disabledGameLevelFeatures;

	UPROPERTY(transient)
	/// <summary>
	/// The default player controller used when a player controller is needed but none available
	/// </summary>
	TArray<APlayerController*> AiPlayerControllers;

	UPROPERTY(VisibleDefaultsOnly, BluePrintReadOnly, Category = ABattleArenaCharacter, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Component that manages cool downs for this game mode
	/// </summary>
	UCoolDownManagerComponent* CoolDownManager;

	UPROPERTY(transient)
	/// <summary>
	/// The soul caster pawn class
	/// </summary>
	TSubclassOf<APawn> SoulCasterPawnClass;

	UPROPERTY(BlueprintReadOnly, Category = ABattleArenaGameMode, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// The list of player definitions for this game
	/// </summary>
	TArray<FPlayerData> PlayerDefinitions;
	
	UPROPERTY()
	/// <summary>
	/// Game phases
	/// </summary>
	TArray<UBattleArenaGamePhase*> Phases;

	UPROPERTY()
	/// <summary>
	/// The spawn queue. Using FSpawnQueueData to avoid that the loaded class from being eliminated by garbage collector.
	/// </summary>
	TArray<FSpawnQueueData> SpawnQueue;

	UPROPERTY()
	/// <summary>
	/// The phase event queue. Using FEventPhaseData to avoid a bug where calling NativeEvent more than one time in a frame causes
	/// the next calls to be missed by UE.
	/// </summary>
	TArray<FPhaseEventData> PhaseEventQueue;	

	/// <summary>
	/// The match start time. -1 while not started
	/// </summary>
	float matchStartTime;

	/// <summary>
	/// Adds the player data for a user (brings the data from the game instance)
	/// </summary>
	/// <param name="playerController">The player controller.</param>
	void CopyPlayerDataFromGameInstance(AController* playerController);

	/// <summary>
	/// Index of the phase where ai will start playing loaded content
	/// </summary>
	/// <returns></returns>
	int aiContentPhaseIndex;

	/// <summary>
	/// Initializes the battle arena.
	/// </summary>
	/// <returns></returns>
	void InitBattleArena();

	/// <summary>
	/// Called after a little delay after match has started.
	/// </summary>	
	void OnMatchStarted_Delayed();

protected:

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = ABattleArenaGameMode)
	/// <summary>
	/// Gets the game phases.
	/// </summary>
	/// <returns></returns>
	TArray<UBattleArenaGamePhase*>& GetPhases();

	/// <summary>
	/// Adds the phase to the phases list
	/// </summary>
	/// <param name="phase">The phase.</param>
	void AddPhase(UBattleArenaGamePhase* phase);

	/// <summary>
	/// Initializes the phases.
	/// </summary>
	virtual void InitPhases(FGameOptionsData gameOptions);

	/// <summary>
	/// Called when the game starts
	/// </summary>
	virtual void BeginPlay() override;

	/// <summary>
	/// Shoulds the spawn at start spot.
	/// </summary>
	/// <param name="player">The player.</param>
	/// <returns></returns>
	virtual bool ShouldSpawnAtStartSpot(AController* player) override;

	/// <summary>
	/// Called every frame
	/// </summary>
	/// <param name="deltaTime">The delta time.</param>
	virtual void Tick(float deltaTime) override;

	/// <summary>
	/// Called after the player has logged in.  Calls OnPlayerJoined
	/// </summary>
	/// <param name="newPlayer">The new player.</param>
	virtual void PostLogin(APlayerController* newPlayer) override;

	/// <summary>
	/// Called to handle player traveling.  Calls OnPlayerJoined
	/// </summary>
	/// <param name="controller">Reference to the player controller.</param>
	//virtual void HandleSeamlessTravelPlayer(AController*& controller) override;

	/// <summary>
	///Override Implementation of ReadyToStartMatch
	/// </summary>
	/// <returns>True if the game mode is ready to start the match</returns>
	virtual bool ReadyToStartMatch_Implementation() override;

	/// <summary>
	/// Choses where the player should start
	/// </summary>
	/// <param name="player">The new player.</param>
	virtual class AActor* ChoosePlayerStart_Implementation(AController* player) override;

	/// <summary>
	/// Gets the minimum number of players to start match.
	/// </summary>
	/// <returns>The minimum number of players to start match.</returns>
	virtual int GetMinPlayersToStartMatch();

	/// <summary>
	/// Gets the count of players that are ready to start the match.
	/// </summary>
	int GetReadyPlayerCount();

	/// <summary>
	/// Gets the default player controller.
	/// </summary>
	/// <returns></returns>
	APlayerController* GetDefaultPlayerController();

	/// <summary>
	/// Registers an AI player controller so it can be disposed of later
	/// </summary>
	/// <param name="value">The value.</param>
	void RegisterAiPlayerController(APlayerController* value);

	/// <summary>
	/// Called when it is decided that the object will be destroyed
	/// </summary>
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = ABattleArenaGameMode, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Gets the game optiosn if player 0 has been already defined.  Otherwise returns null
	/// </summary>
	FGameOptionsData GetGameOptions(bool& success);

	/// <summary>
	/// Used by multiplayer server flow to get a notification that the server travel has finished.
	/// </summary>
	virtual void PostSeamlessTravel() override;

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="ABattleArenaGameMode"/> class.
	/// </summary>
	ABattleArenaGameMode();	

	/// <summary>
	/// Gets the soul caster pawn class.
	/// </summary>
	/// <returns></returns>
	TSubclassOf<APawn> GetSoulCasterPawnClass(FAllegianceData allegiance);

	/// <summary>
	/// Adds an AI player definition
	/// </summary>
	/// <param name="vsPlayer">The player that the AI will play against.</param>
	/// <param name="mainPlayerController">Controller used for http calls.</param>
	virtual void AddPlayerDefinitionsForAI(FPlayerData& vsPlayer, APlayerController* mainPlayerController);

	/// <summary>
	/// ReSpawns the player.
	/// </summary>
	/// <param name="controller">The controller.</param>
	/// <param name="gameStartFlow">Indicates whether respawn happens during game start.
	/// Note: Be very careful when respawning casters in the middle of the game. Here be dragons.</param>
	void ReSpawnPlayer(APlayerController* controller, bool gameStartFlow = true);

	/// <summary>
	/// Checks the game end condition.
	/// </summary>
	/// <returns>
	/// Returns true if the game should end.  False Otherwise
	/// </returns>
	virtual bool CheckGameEndCondition();

	/// <summary>
	/// Gets the first phase.
	/// </summary>
	/// <returns>A game phase</returns>
	UBattleArenaGamePhase& GetFirstPhase();

	/// <summary>
	/// Gets the next phase.
	/// </summary>
	/// <param name="currentPhase">The current phase.</param>
	/// <returns>
	/// A game phase
	/// </returns>
	virtual UBattleArenaGamePhase* GetNextPhase(UBattleArenaGamePhase& currentPhase);

	/// <summary>
	/// Ends the match.
	/// </summary>
	virtual void EndMatch();

	/// <summary>
	/// Adds an actor to the current spawn queue.  It is required that the actor has been spawned in deferred mode.
	/// The game mode will finish spawning it in the next tick
	/// </summary>
	/// <param name="actor">The actor.</param>
	void AddToSpawnQueue(AActor* actor);

	/// <summary>
	/// Gets the list of player defintions.
	/// </summary>
	/// <returns></returns>
	TArray<FPlayerData>& GetPlayerDefinitions();

	/// <summary>
	/// Called when the player controller has finished loading its deck.
	/// </summary>
	/// <param name="playerController">The player controller.</param>
	/// <param name="deck">The deck.</param>
	virtual void OnGetDeckCompleted(APlayerController* playerController, FDeckData deck);

	/// <summary>
	/// Gets the game result.
	/// </summary>
	/// <param name="player0Hp">The player Hp at the end of the game. INT_MIN if the game was unable to figure out HP</param>
	/// <param name="player1Hp">The player Hp at the end of the game. INT_MIN if the game was unable to figure out HP</param>
	virtual EGameResult GetGameResult(
		int& player0Hp, 
		int& player1Hp, 
		EAlternativeWinCondition& player0WinCondition, 
		EAlternativeWinCondition& player1WinCondition);

	/// <summary>
	/// Creates the ai deck from player data.
	/// </summary>
	/// <param name="battleArena">The battle arena.  This gets passed because at this point the battle arena may not be assigned to the game mode</param>
	/// <param name="playerData">The player data.</param>
	/// <param name="toReturn">Ai rules will be added to this list.</param>
	virtual void CreateAiRulesFromPlayerData(IBattleArena& battleArena, FPlayerData playerData, TListGroupTemplate<TPlayerAiRule>& toReturn);

	/// <summary>
	/// Creates the ai player.
	/// </summary>
	/// <param name="playerData">The player data.</param>
	/// <returns></returns>
	virtual IAIPlayer* CreateAiPlayer(FPlayerData& playerData);

	/// <summary>
	/// Gets the match start time
	/// </summary>
	float GetMatchStartTime();

	/// <summary>
	/// Gets the cool down manager component
	/// </summary>
	/// <returns></returns>
	UCoolDownManagerComponent* GetCoolDownManager();

	UFUNCTION(BluePrintCallable, Category = ABattleArenaGameMode)
	/// <summary>
	/// Triggers an event that will be handled by the current phase
	/// </summary>
	virtual void TriggerPhaseEvent(FString eventName, UObject* target = nullptr);

	UFUNCTION(BluePrintCallable, Category = ABattleArenaGameMode)
	/// <summary>
	/// Triggers a level event.
	/// </summary>
	void TriggerLevelEvent(FName eventName);

	/// <summary>
	/// Gets the index of the phase where ai will start playing loaded content
	/// </summary>
	/// <returns></returns>
	int GetAiContentPhaseIndex();

	/// <summary>
	/// Sets the index of the phase where ai will start playing loaded content
	/// </summary>
	/// <returns></returns>
	void SetAiContentPhaseIndex(int index);

	UFUNCTION(BluePrintCallable, BlueprintPure, Category = ABattleArenaGameMode)
	/// <summary>
	/// Returns true if the underlying battle arena match is in progress
	/// </summary>
	bool IsBattleArenaGameInProgress();

	UFUNCTION(BluePrintCallable, BlueprintPure, Category = ABattleArenaGameMode)
	/// <summary>
	/// Returns true if the match results submission has been confirmed
	/// </summary>
	bool GetIsMatchResultsSubmissionConfirmed();

	UFUNCTION(BluePrintCallable, BlueprintPure, Category = ABattleArenaGameMode)
	/// <summary>
	/// Gets the match result data
	/// </summary>
	FMatchResultData GetMatchResultData();

	UFUNCTION(BluePrintCallable, Category = ABattleArenaGameMode)
	/// <summary>
	/// Updates the player deck id.
	/// This is used by the deck/home builder to update the deck that is going to test/enable.
	/// </summary>
	/// <param name="deckId">The new deck id.</param>
	/// <param name="playerIndex">The index of the player to update.</param>
	void UpdatePlayerDeckId(FString deckId, int playerIndex);

	UFUNCTION()
	/// <summary>
	/// Called when match results submission has been completed
	/// </summary>
	void OnSubmitMatchResultsCompleted(bool success);

	/// <summary>
	/// Returns default pawn class for given controller
	/// </summary>
	/// <param name="InController">The in controller.</param>
	/// <returns></returns>
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	/// <summary>
	/// Overrides restart player so no pawn is spawned for neutral player
	/// </summary>	
	virtual void RestartPlayer(AController* NewPlayer) override;

	UFUNCTION(BlueprintCallable, Category = ABattleArenaGameMode)
	/// <summary>
	/// Gets the building locator related to the soulWell.
	/// If the soulWell is null returns the totem building locator
	/// </summary>
	/// <returns></returns>
	class ABuildingLocator* GetBuildingLocator(ASoulWellActor* soulWellActor, bool totemBuildings);	
	
	UFUNCTION(BluePrintCallable, Category = ABattleArenaGameMode)
	/// <summary>
	/// Performs a battle arena reset (Dispells all spells and removes all creatures)
	/// </summary>
	/// <param name="creatureDespawnFx">The creature despawn fx.</param>
	void ResetBattleArena(TSubclassOf<class AFxCollection> creatureDespawnFx);

	UFUNCTION(BluePrintCallable, Category = ABattleArenaGameMode)
	/// <summary>
	/// Enables/Disables game level features
	/// </summary>	
	void SetGameLevelFeatureEnabled(EConfigurableGameLevelFeature gameLevelFeature, bool enabled);

	UFUNCTION(BluePrintCallable, BluePrintPure, Category = ABattleArenaGameMode)
	/// <summary>
	/// Gets a value indicating whether a game level feature is enabled.
	/// </summary>	
	bool GetGameLevelFeatureEnabled(EConfigurableGameLevelFeature gameLevelFeature);

	TSet<EConfigurableGameLevelFeature> DisabledGameLevelFeatures;

	UFUNCTION(BluePrintCallable, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Gets the pipeline mask for spells
	/// </summary>
	void GetSpellPipelineMask(UPARAM(meta = (Bitmask, BitmaskEnum = ESpellPipelineMask)) int& value);

	UFUNCTION(BluePrintCallable, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Sets the pipeline mask for spells
	/// </summary>
	void SetSpellPipelineMask(UPARAM(meta = (Bitmask, BitmaskEnum = ESpellPipelineMask)) int value);

	UFUNCTION(BluePrintCallable, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Ends the current running phase (Independently of the CheckPhaseEnd check).
	/// </summary>
	void EndCurrentPhase();

	/// <summary>
	/// Gets the min distance of creatures to the enemy totem.
	/// </summary>
	float GetMinDistanceToTotem();

	UFUNCTION(BluePrintCallable, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Gets the creatures in area.
	/// </summary>
	/// <param name="location">The location.</param>
	/// <param name="orientation">The orientation.</param>
	/// <param name="areaType">Type of the area.</param>
	/// <param name="area">The area in world units.</param>
	/// <param name="results">The method will add the results to this list</param>
	void GetCreaturesInArea(FVector location, FRotator orientation, EAreaType areaType, float area, TArray<ABattleArenaCharacter*>& results);

	UFUNCTION(BluePrintCallable, Category = ABattleArenaLevelScriptActor)
    /// <summary>
    /// Gets the enemy creatures in area.
    /// </summary>
    /// <param name="location">The location.</param>
    /// <param name="orientation">The orientation.</param>
    /// <param name="areaType">Type of the area.</param>
    /// <param name="area">The area in world units.</param>
    /// <param name="excludeUnreachable">Filter out unreachable creatures.</param>
    /// <param name="results">The method will add the results to this list</param>
    void GetEnemyCreaturesInArea(
    	ABattleArenaCharacter* character,
    	FVector location,
    	FRotator orientation,
    	EAreaType areaType,
    	float area,
    	bool excludeUnreachable,
    	TArray<ABattleArenaCharacter*>& results);

	UFUNCTION(BluePrintCallable, Category = ABattleArenaLevelScriptActor)
    /// <summary>
    /// Gets the ally creatures in area.
    /// </summary>
    /// <param name="location">The location.</param>
    /// <param name="orientation">The orientation.</param>
    /// <param name="areaType">Type of the area.</param>
    /// <param name="area">The area in world units.</param>
    /// <param name="excludeUnreachable">Filter out unreachable creatures.</param>
    /// <param name="results">The method will add the results to this list</param>
    void GetAllyCreaturesInArea(
    	ABattleArenaCharacter* character,
    	FVector location,
    	FRotator orientation,
    	EAreaType areaType,
    	float area,
    	bool excludeUnreachable,
    	TArray<ABattleArenaCharacter*>& results);

	UFUNCTION(BluePrintCallable, Category = ABattleArenaLevelScriptActor)
    /// <summary>
    /// Gets all reachable enemies.
    /// </summary>
    /// <param name="character">Character used to do the reachability test.</param>
    /// <param name="results">The method will add the results to this list</param>
    void GetReachableEnemies(ABattleArenaCharacter* character, TArray<ABattleArenaCharacter*>& results);

	/// <summary>
	/// Called when a new player joins the battle arena game
	/// </summary>
	/// <param name="newPlayer">The new player.</param>
	void OnPlayerJoined(AController* newPlayer);

	/// <summary>
	/// Finalizes an instance of the <see cref="ABattleArenaGameMode"/> class.
	/// </summary>
	virtual ~ABattleArenaGameMode();
};