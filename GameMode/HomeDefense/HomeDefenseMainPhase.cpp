// <copyright file="HomeDefenseMainPhase.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "GameMode/BattleArenaGameMode.h"
#include "Cosmetic/ClientTextNotificationEffect.h"
#include "Character/SoulCaster/Actor/SoulShardsCharacter.h"
#include "HomeDefenseMainPhase.h"

UHomeDefenseMainPhase::UHomeDefenseMainPhase()
{
	this->SetPhaseName(TEXT("Invasion Phase"));
	this->SetPhaseGoalText(TEXT("Defeat enemy in {time} minutes"));
	this->HandicapMultiplier = 1.0f;
	this->CheckHitPointsEndPhaseCondition = true;
}

int UHomeDefenseMainPhase::GetPhaseDuration()
{
	return (int)((float)this->InvasionPhaseDuration * this->HandicapMultiplier);
}

void UHomeDefenseMainPhase::SetHandicapMultiplier(float handicapMultiplier)
{
	this->HandicapMultiplier = handicapMultiplier;
}

bool UHomeDefenseMainPhase::CheckPhaseEndCondition_Implementation()
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
				int i = 0;
				for (it.GoFirst(); it.Actual() != nullptr; it.GoNext())
				{
					TDuelPlayer* actual = it.Actual();
					FPlayerData& playerDefinition = actual->GetPlayerDefinition();

					// End game condition doesn't check neutral players
					if (!playerDefinition.Allegiance.IsNeutral())
					{
						ASoulShardsCharacter* soulCaster = actual->GetSoulCaster();
						if (i != 0 || this->CheckHitPointsEndPhaseCondition)
						{
							if ((soulCaster == nullptr || (soulCaster->GetD20Character() && soulCaster->GetD20Character()->GetBaseStats()->GetHitPoints()->ToInt() <= 0)))
							{
								return true;
							}
						}
					}
					i++;
				}
			}
		}

	}

	return timeLimitCondition;
}

UHomeDefenseMainPhase::~UHomeDefenseMainPhase()
{
}


