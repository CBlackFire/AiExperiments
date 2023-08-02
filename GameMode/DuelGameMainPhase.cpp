// <copyright file="DuelGameMainPhase.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "Character/SoulCaster/Actor/SoulShardsCharacter.h"
#include "GameMode/BattleArenaGameMode.h"
#include "DuelGameMainPhase.h"

UDuelGameMainPhase::UDuelGameMainPhase()
{
}

bool UDuelGameMainPhase::CheckGameEndCondition_Implementation()
{
	return false;
}

bool UDuelGameMainPhase::CheckPhaseEndCondition_Implementation()
{
	bool timeLimitCondition = Super::CheckPhaseEndCondition_Implementation();
	if (!timeLimitCondition)
	{
		ABattleArenaGameMode* gameMode = Cast<ABattleArenaGameMode>(this->GetWorld()->GetAuthGameMode());
		if (gameMode)
		{
			TServerBattleArena* serverBattleArena = static_cast<TServerBattleArena*>(gameMode->GetBattleArena());
			if (serverBattleArena)
			{
				TSmartListGroupIteratorTemplate<TDuelPlayer, false> it(serverBattleArena->players);
				for (it.GoFirst(); it.Actual() != nullptr; it.GoNext())
				{
					TDuelPlayer* actual = it.Actual();
					FPlayerData& playerDefinition = actual->GetPlayerDefinition();

					// End game condition doesn't check neutral players
					if (!playerDefinition.Allegiance.IsNeutral())
					{
						ASoulShardsCharacter* soulCaster = actual->GetSoulCaster();
						if (soulCaster == nullptr)
						{
							return true;
						}

						if ((soulCaster->GetD20Character() && soulCaster->GetD20Character()->GetBaseStats()->GetHitPoints()->ToInt() <= 0))
						{
							return true;
						}

						if (soulCaster->GetAlternativeWinState() != EAlternativeWinCondition::AWCNoWin)
						{
							return true;
						}
					}
				}
			}
		}

	}

	return timeLimitCondition;
}

UDuelGameMainPhase::~UDuelGameMainPhase()
{
}