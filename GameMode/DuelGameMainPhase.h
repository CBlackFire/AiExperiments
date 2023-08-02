// <copyright file="DuelGameMainPhase.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/BattleArenaGamePhase.h"
#include "DuelGameMainPhase.generated.h"

UCLASS()
/// <summary>
/// Defines the main phase for a duel game type
/// </summary>
class SOULSHARDS_API UDuelGameMainPhase : public UBattleArenaGamePhase
{
	GENERATED_BODY()
	
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="TDuelGameMainPhase"/> class.
	/// </summary>
	UDuelGameMainPhase();

	/// <summary>
	/// Checks the game end condition.
	/// </summary>
	/// <returns>
	/// Returns true if the game should end.  False Otherwise
	/// </returns>
	virtual bool CheckGameEndCondition_Implementation() override;

	/// <summary>
	/// Checks the phase end condition.
	/// </summary>
	/// <returns>
	/// Returns true if the phase should end.  False Otherwise
	/// </returns>
	bool CheckPhaseEndCondition_Implementation() override;

	/// <summary>
	/// Finalizes an instance of the <see cref="TDuelGameMainPhase"/> class.
	/// </summary>
	virtual ~UDuelGameMainPhase();
	
	
};
