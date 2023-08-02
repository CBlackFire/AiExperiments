// <copyright file="TNightmareChallengeAiPlayer.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "Character/AiController/BattleArenaAIController.h"
#include "BattleEngine/BattleArena/TServerBattleArena.hpp"
#include "TNightmareChallengeAiPlayer.hpp"

V_IMPLEMENT_RUNTIMECLASS(TNightmareChallengeAiPlayer, TAIPlayer, 0);

TNightmareChallengeAiPlayer::TNightmareChallengeAiPlayer()
	: TAIPlayer()
{
	checkf(false, TEXT("Invalid constructor"));
};

TNightmareChallengeAiPlayer::TNightmareChallengeAiPlayer(IBattleArena& battleArena)
	: TAIPlayer(battleArena)
{
};

TNightmareChallengeAiPlayer::TNightmareChallengeAiPlayer(TNightmareChallengeAiPlayer& toCopy, PBaseObject cloneContext)
	: TAIPlayer(toCopy, cloneContext)
{
};

TBaseObject& TNightmareChallengeAiPlayer::Clone(PBaseObject cloneContext)
{
	TBaseObject& clone = *new TNightmareChallengeAiPlayer(*this, cloneContext);
	clone.SetName(GetName());
	return clone;
};

TPlayerAiRule* TNightmareChallengeAiPlayer::GetNextAiRule()
{
	// Same as home defense but when rules ar exhausted, it goes back to first rule in phase 1
	TPlayerAiRule* nextRule = TAIPlayer::GetNextAiRule();
	if (nextRule == nullptr)
	{
		this->SetCurrentPlayerAiRuleIndex(0);
		nextRule = TAIPlayer::GetNextAiRule();
		while (nextRule && nextRule->GetDeckPhase() < 1)
		{
			nextRule = TAIPlayer::GetNextAiRule();
		}
	}

	return nextRule;
}