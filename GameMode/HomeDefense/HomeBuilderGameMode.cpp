// <copyright file="HomeBuilderGameMode.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "Character/SoulCaster/Actor/SoulShardsCharacter.h"
#include "HomeBuilderGameMode.h"

AHomeBuilderGameMode::AHomeBuilderGameMode()
{
}

EGameResult AHomeBuilderGameMode::GetGameResult(
	int& player0HitPoints, 
	int& player1HitPoints,
	EAlternativeWinCondition& player0WinCondition,
	EAlternativeWinCondition& player1WinCondition)
{
	TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());

	TSmartListGroupIteratorTemplate<TDuelPlayer, false> itPlayer(battleArena->players);
	TDuelPlayer* player0 = itPlayer.At(0);
	TDuelPlayer* player1 = itPlayer.At(1);

	ASoulShardsCharacter* player0Caster = player0->GetSoulCaster();
	ASoulShardsCharacter* player1Caster = player1->GetSoulCaster();

	bool player0Died = player0Caster == nullptr;
	bool player1Died = player1Caster == nullptr;

	player0HitPoints = -1;
	player1HitPoints = -1;

	player0WinCondition = EAlternativeWinCondition::AWCNoWin;
	player1WinCondition = EAlternativeWinCondition::AWCNoWin;

	if (player0Caster && player0Caster->GetD20Character())
	{
		player0HitPoints = player0Caster->GetD20Character()->GetStats()->GetHitPoints()->GetValue()->ToInt();
		player0Died = player0HitPoints <= 0;
		player0WinCondition = player0Caster->GetAlternativeWinState();
	}

	if (player1Caster && player1Caster->GetD20Character())
	{
		player1HitPoints = player1Caster->GetD20Character()->GetStats()->GetHitPoints()->GetValue()->ToInt();
		player1Died = player1HitPoints <= 0;
		player0WinCondition = player1Caster->GetAlternativeWinState();
	}

	// if home defense survives, it's a victory for home defense
	if (!player1Died)
	{
		// If there are no alternative win conditions this is a win for home defense
		if (player0WinCondition == EAlternativeWinCondition::AWCNoWin)
		{
			return EGameResult::EGRPlayer1Win;
		}
		else
		{
			// The player can still win by alternative condition.
			return EGameResult::EGRPlayer0Win;
		}
	}
	
	return EGameResult::EGRPlayer0Win;
}

bool AHomeBuilderGameMode::ShouldRecordReplay()
{
	return false;
}