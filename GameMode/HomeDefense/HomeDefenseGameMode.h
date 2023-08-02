// <copyright file="HomeDefenseGameMode.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/BattleArenaGameMode.h"
#include "HomeDefenseGameMode.generated.h"

UCLASS()
/// <summary>
/// Home Defense Game Mode.
/// </summary>
/// <seealso cref="ABattleArenaGameMode" />
class SOULSHARDS_API AHomeDefenseGameMode : public ABattleArenaGameMode
{
	GENERATED_BODY()

protected:

	/// <summary>
	/// Initializes the phases.
	/// </summary>
	virtual void InitPhases(FGameOptionsData gameOptions) override;

public:

	/// <summary>
    /// Should be overriden by base classes to indicate if the game mode wants the game to be recorded.
    /// </summary>
    /// <returns>
    /// True.
    /// </returns>
    virtual bool ShouldRecordReplay() override;	

	/// <summary>
	/// Adds additional player definitions for the specified game.
	/// </summary>
	virtual void AddPlayerDefinitionsForAI(FPlayerData& vsPlayer, APlayerController* mainPlayerController) override;

	/// <summary>
	/// Called when the player controller has finished loading its deck.
	/// </summary>
	/// <param name="playerController">The player controller.</param>
	/// <param name="deck">The deck.</param>
	virtual void OnGetDeckCompleted(APlayerController* playerController, FDeckData deck) override;

	UFUNCTION()
	/// <summary>
	/// Called when the get deck id is retrieved for random content games.
	/// </summary>
	/// <param name="contentId">Content id.</param>
	/// <param name="gameType">The deck.</param>
	void OnGetDeckIdCompleted(FString contentId, EGameType gameType);

	/// <summary>
	/// Gets the game result.
	/// </summary>
	virtual EGameResult GetGameResult(
		int& player0Hp, 
		int& player1Hp,
		EAlternativeWinCondition& player0WinCondition,
		EAlternativeWinCondition& player1WinCondition) override;

	/// <summary>
	/// Initializes a new instance of the <see cref="AHomeDefenseGameMode"/> class.
	/// </summary>
	AHomeDefenseGameMode();
};
