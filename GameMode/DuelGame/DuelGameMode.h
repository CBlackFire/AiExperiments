// <copyright file="DuelGameMode.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/BattleArenaGameMode.h"
#include "DuelGameMode.generated.h"

UCLASS()
/// <summary>
/// Represents a duel game
/// </summary>
/// <seealso cref="ABattleArenaGameMode" />
class SOULSHARDS_API ADuelGameMode : public ABattleArenaGameMode
{
	GENERATED_BODY()
	
protected:

	/// <summary>
	/// Initializes the phases.
	/// </summary>
	virtual void InitPhases(FGameOptionsData gameOptions) override;

	/// <summary>
	/// Gets the minimum number of players to start match.
	/// </summary>
	/// <returns>The minimum number of players to start match.</returns>
	virtual int GetMinPlayersToStartMatch() override;

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="AHomeDefenseGameMode"/> class.
	/// </summary>
	ADuelGameMode();

	/// <summary>
	/// Should be overriden by base classes to indicate if the game mode wants the game to be recorded.
	/// </summary>
	/// <returns>
	/// True.
	/// </returns>
	virtual bool ShouldRecordReplay() override;
};
