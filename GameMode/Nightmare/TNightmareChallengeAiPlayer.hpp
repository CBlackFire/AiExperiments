// <copyright file="TNightmareChallengeAiPlayer.hpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "AiEngine/AiEngine/TAIPlayer.hpp"

/// <summary>
/// Nightmare Ai Player
/// </summary>
class TNightmareChallengeAiPlayer : public TAIPlayer
{
	V_DECLARE_SERIAL_DLLEXP(TNightmareChallengeAiPlayer, SOULSHARDS_API)

protected:
	/// <summary>
	/// Selects the next ai rule.
	/// </summary>
	virtual TPlayerAiRule* GetNextAiRule() override;

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="TNightmareChallengeAiPlayer"/> class.
	/// </summary>
	TNightmareChallengeAiPlayer();

	/// <summary>
	/// Initializes a new instance of the <see cref="TNightmareChallengeAiPlayer"/> class.
	/// </summary>
	TNightmareChallengeAiPlayer(IBattleArena& battleArena);

	// Constructor del TNightmareChallengeAiPlayer, apartir de otro TNightmareChallengeAiPlayer, hace que este objeto sea un "Clon" del objeto 'toCopy'.
	TNightmareChallengeAiPlayer(TNightmareChallengeAiPlayer& toCopy, PBaseObject CloneContext = nullptr);

	/* Crea un nuevo objeto a partir de este */
	virtual TBaseObject& Clone(PBaseObject CloneContext = nullptr);
};