// <copyright file="NightmareGameMode.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/Data/NightmareProgressionWorldEnchantContainer.h"
#include "GameMode/Scripted/ScriptedGameMode.h"
#include "NightmareGameMode.generated.h"

UCLASS()
/// <summary>
/// Nightmare Game Mode
/// </summary>
/// <seealso cref="ABattleArenaGameMode" />
class SOULSHARDS_API ANightmareGameMode : public AScriptedGameMode
{
	GENERATED_BODY()

private:

	UPROPERTY(BlueprintReadOnly, Category = ANightmareGameMode, meta = (AllowPrivateAccess = "true"))
	int Level;

	UPROPERTY(BlueprintReadOnly, transient, Category = ANightmareGameMode, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// The nightmare progression world enchant container
	/// </summary>
	UNightmareProgressionWorldEnchantContainer* NightmareProgressionWorldEnchantContainer;

	UPROPERTY(BlueprintReadWrite, transient, Category = ANightmareGameMode, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// The cost necessary to pay for next main phase cast
	/// </summary>
	float SoulPointsDebt;

	UPROPERTY(BlueprintReadWrite, transient, Category = ANightmareGameMode, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Amount of defensive Soul Points earned
	/// </summary>
	float DefenseSoulPointsEarned;
	
	/// <summary>
	/// Get nightmare progression container.
	/// </summary>
	UNightmareProgressionWorldEnchantContainer* GetNightmareProgressionWorldEnchantContainer();

protected:

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="ANightmareGameMode"/> class.
	/// </summary>
	ANightmareGameMode();	

	/// <summary>
	/// Gets the game result.
	/// </summary>
	virtual EGameResult GetGameResult(
		int& player0Hp, 
		int& player1Hp,
		EAlternativeWinCondition& player0WinCondition,
		EAlternativeWinCondition& player1WinCondition) override;

	/// <summary>
	/// Called when the player controller has finished loading its deck.
	/// </summary>
	/// <param name="playerController">The player controller.</param>
	/// <param name="deck">The deck.</param>
	virtual void OnGetDeckCompleted(APlayerController* playerController, FDeckData deck) override;

	/// <summary>
	/// Should be overriden by base classes to indicate if the game mode wants the game to be recorded.
	/// </summary>
	/// <returns>
	/// True.
	/// </returns>
	virtual bool ShouldRecordReplay() override;
};
