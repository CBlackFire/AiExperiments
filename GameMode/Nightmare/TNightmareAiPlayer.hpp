// <copyright file="TNightmareAiPlayer.hpp" company="Psiontech">
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
class TNightmareAiPlayer : public TAIPlayer
{
	V_DECLARE_SERIAL_DLLEXP(TNightmareAiPlayer, SOULSHARDS_API)

protected:
	/// <summary>
	/// Selects the next ai rule.
	/// </summary>
	virtual TPlayerAiRule* GetNextAiRule() override;

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="TNightmareAiPlayer"/> class.
	/// </summary>
	TNightmareAiPlayer();

	/// <summary>
	/// Initializes a new instance of the <see cref="TNightmareAiPlayer"/> class.
	/// </summary>
	TNightmareAiPlayer(IBattleArena& battleArena);

	// Constructor del TNightmareAiPlayer, apartir de otro TNightmareAiPlayer, hace que este objeto sea un "Clon" del objeto 'toCopy'.
	TNightmareAiPlayer(TNightmareAiPlayer& toCopy, PBaseObject CloneContext = nullptr);

	/* Crea un nuevo objeto a partir de este */
	virtual TBaseObject& Clone(PBaseObject CloneContext = nullptr);

	/// <summary>
	/// Selects a valid spell target por an AIRule
	/// </summary>
	virtual AActor* SelectTarget(TPlayerAiRule* rule, USpellTarget* spellTarget) override;

};