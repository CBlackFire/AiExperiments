// <copyright file="ScriptedGamePhase.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/TimeLimitGamePhase.h"
#include "GameMode/Data/PhaseStartData.h"
#include "BattleEngine/Data/AllegianceData.h"
#include "Player/Data/DeckData.h"
#include "ScriptedGamePhase.generated.h"

UCLASS(BlueprintType, Blueprintable)
/// <summary>
/// Defines a scripted game phase that can be extended in blueprints
/// </summary>
class SOULSHARDS_API UScriptedGamePhase : public UTimeLimitGamePhase
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UScriptedGamePhase, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Defines the duration of the phase.
	/// </summary>
	int PhaseDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UScriptedGamePhase, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Indicates whether this phase can coexist in parallel with a trigger phase
	/// </summary>
	bool AllowTriggerPhase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UScriptedGamePhase, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Data that indicates the desired state of the game at the beginning of the game.
	/// </summary>
	FPhaseStartData PhaseStartData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UScriptedGamePhase, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Data that indicates the desired state of the game at the beginning of the game.
	/// </summary>
	FPhaseStartData PhaseEndData;

	/// <summary>
	/// Executes the phase data
	/// </summary>
	void ExecutePhaseData(FPhaseStartData phaseData);

public:
	/// <summary>
	/// Initializes a new instance of the <see cref="UScriptedGamePhase"/> class.
	/// </summary>
	UScriptedGamePhase();

	/// <summary>
	/// Indicates whether the current phase 
	/// </summary>
	virtual bool GetAllowTriggerPhase();
	
	/// <summary>
	/// Returns the phase duration in seconds
	/// </summary>
	virtual int GetPhaseDuration();

	/// <summary>
	/// Called when the phase starts
	/// </summary>
	virtual void OnPhaseStart_Implementation();

	UFUNCTION(BlueprintCallable, Category = UScriptedGamePhase)
	/// <summary>
	/// Sets the view target to the specified camera
	/// </summary>
	void SetViewTarget(const FName& cameraTag);

	UFUNCTION(BlueprintPure, BluePrintCallable, Category = UScriptedGamePhase)
	/// <summary>
	/// Gets all the soulwells
	/// </summary>
	/// <returns>A soulwell array</returns>
	TArray<class ASoulWellActor*> GetSoulWells();

	UFUNCTION(BlueprintPure, BluePrintCallable, Category = UScriptedGamePhase)
	/// <summary>
	/// Get the SoulWell if it's enabled for the player.  Otherwise looks for other enabled soul wells, first of of the same type
	/// </summary>	
	class ASoulWellActor* GetEnabledSoulWell(FAllegianceData allegiance, ESoulWellType soulWellType, int soulWellIndex, bool bypassEnabled = false);

	UFUNCTION(BlueprintPure, BluePrintCallable, Category = UScriptedGamePhase)
	/// <summary>
	/// Gets the soul caster for a player
	/// </summary>
	/// <param name="caster">The caster.</param>
	class ASoulShardsCharacter* GetSoulCaster(int playerIndex);

	UFUNCTION(BlueprintPure, BluePrintCallable, Category = UScriptedGamePhase)
	/// <summary>
	/// Gets creatures by zone (from the perspective of the AI player)
	/// </summary>
	/// <param name="zoneType">Type of the zone.</param>
	/// <param name="partyFilter">Type of the party filter.</param>
	/// <returns>A BattleArenaCharacter array</returns>
	TArray<class ABattleArenaCharacter*> GetCreaturesByZone(EBattleArenaZoneType zoneType, EPartyFilterType partyFilter = EPartyFilterType::PFTAny);

	UFUNCTION(BluePrintCallable, Category = UScriptedGamePhase)
	/// <summary>
	/// Destroys a creature
	/// </summary>
	/// <param name="creature">Creature.</param>
	void DestroyCreature(ABattleArenaCharacter* creature);

	UFUNCTION(BluePrintCallable, Category = UScriptedGamePhase)
	/// <summary>
	/// Sets a deck as the source for Ai rules
	/// </summary>
	/// <param name="playerIndex">Index of the player.</param>
	/// <param name="deck">Deck to create AiRules from.</param>
	void SetAiRulesFromDeck(int playerIndex, FDeckData deck);

	UFUNCTION(BluePrintCallable, Category = UScriptedGamePhase)
	/// <summary>
	/// Sets the player deck
	/// </summary>
	/// <param name="playerIndex">Index of the player.</param>
	/// <param name="deck">The deck of the player.</param>
	void SetPlayerDeck(int playerIndex, FDeckData deck);

	UFUNCTION(BluePrintCallable, Category = UScriptedGamePhase)
	/// <summary>
	/// Returns a creature that was previously removed to the game
	/// </summary>
	ABattleArenaCharacter* ReturnCreatureToGame(FGuid removalId, TSubclassOf<class AFxCollection> fxOverride);

	/// <summary>
	/// Finalizes an instance of the <see cref="TScriptedGamePhase"/> class.
	/// </summary>
	virtual ~UScriptedGamePhase();
};
