// <copyright file="HomeBuilderGameMode.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/Data/NightmareProgressionWorldEnchantContainer.h"
#include "GameMode/Scripted/ScriptedGameMode.h"
#include "HomeBuilderGameMode.generated.h"

UCLASS()
/// <summary>
/// Home Builder Game Mode
/// </summary>
/// <seealso cref="ABattleArenaGameMode" />
class SOULSHARDS_API AHomeBuilderGameMode : public AScriptedGameMode
{
	GENERATED_BODY()

private:

protected:

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="ANightmareGameMode"/> class.
	/// </summary>
	AHomeBuilderGameMode();	

	/// <summary>
	/// Gets the game result.
	/// </summary>
	virtual EGameResult GetGameResult(
		int& player0Hp, 
		int& player1Hp,
		EAlternativeWinCondition& player0WinCondition,
		EAlternativeWinCondition& player1WinCondition) override;

	/// <summary>
	/// Should be overriden by base classes to indicate if the game mode wants the game to be recorded.
	/// </summary>
	/// <returns>
	/// False.
	/// </returns>
	virtual bool ShouldRecordReplay() override;
	
};
