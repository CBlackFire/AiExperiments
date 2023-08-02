// <copyright file="TNightmareAiPlayer.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "Character/AiController/BattleArenaAIController.h"
#include "Character/SoulCaster/Actor/SoulShardsCharacter.h"
#include "BattleEngine/BattleArena/TServerBattleArena.hpp"
#include "TNightmareAiPlayer.hpp"


V_IMPLEMENT_RUNTIMECLASS(TNightmareAiPlayer, TAIPlayer, 0);

TNightmareAiPlayer::TNightmareAiPlayer()
	: TAIPlayer()
{
	checkf(false, TEXT("Invalid constructor"));
};

TNightmareAiPlayer::TNightmareAiPlayer(IBattleArena& battleArena)
	: TAIPlayer(battleArena)
{
};

TNightmareAiPlayer::TNightmareAiPlayer(TNightmareAiPlayer& toCopy, PBaseObject cloneContext)
	: TAIPlayer(toCopy, cloneContext)
{
};

TBaseObject& TNightmareAiPlayer::Clone(PBaseObject cloneContext)
{
	TBaseObject& clone = *new TNightmareAiPlayer(*this, cloneContext);
	clone.SetName(GetName());
	return clone;
};

TPlayerAiRule* TNightmareAiPlayer::GetNextAiRule()
{
	TServerBattleArena* serverBattleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	UBattleArenaGamePhase* phase = serverBattleArena->GetCurrentGamePhase();
	TListGroupTemplate<TPlayerAiRule>* aiPlayerRules = this->GetPlayerAiRules();
	TListGroupIteratorTemplate<TPlayerAiRule> it(*aiPlayerRules);

	if (phase == nullptr || phase->GetPhaseIndex() == 0)
	{
		TPlayerAiRule* nextRule = TAIPlayer::GetNextAiRule();
		if (nextRule)
		{
			if (nextRule->GetDeckPhase() == 0)
			{
				return nextRule;
			}
			else
			{
				int currentCardPhase = 0;
				while (currentCardPhase == 0 && it.GoFirst() != nullptr)
				{
					TPlayerAiRule* actual = it.GoFirst();
					currentCardPhase = actual->GetDeckPhase();
					if (currentCardPhase == 0)
					{
						aiPlayerRules->ExtractFirst();
					}
				}
			}
		}
	}

	if (aiPlayerRules->Count > 0)
	{
		int newRule = rand() % aiPlayerRules->Count;
		return it.At(newRule);
	}

	return nullptr;
}

AActor* TNightmareAiPlayer::SelectTarget(TPlayerAiRule* rule, USpellTarget* spellTarget)
{
	TServerBattleArena& serverBattleArena = static_cast<TServerBattleArena&>(*this->GetBattleArena());
	PDuelPlayer player = serverBattleArena.GetAiChallengerPlayer();
	check(player);
	ASoulShardsCharacter* caster = player->GetSoulCaster();
	check(caster);
	FAllegianceData casterAllegiance = caster->GetAllegiance();
	
	USpellTargetSelector* targetSelector = spellTarget ? spellTarget->GetTargetSelector() : nullptr;
	if (targetSelector)
	{
		TArray<AActor*> targets = targetSelector->GetValidTargets();
		if (targets.Num() > 0)
		{
			// We select random targets depending on life.  If life is high the chance of selecting a random target is high.  After
			// life gets 0.25, the AI will defend whenever it needs to.
			float chanceToSelectRandomTarget = FMath::Clamp<float>((caster->GetRemainingLifeFraction() - 0.25f)*1.33f, 0.0f, 1.0f);

			// Roll to check if a random target should be selected
			int randomTargetSelectRoll = (float)rand() / (float)RAND_MAX;
			
			if (randomTargetSelectRoll <= chanceToSelectRandomTarget)
			{
				int maxThreatTargetIndex = -1;
				int maxThreat = 0;
				int index = 0;
				TMap<ASoulWellActor*, int> soulWellThreatMap;

				// If we need to defend, we'll need to evaluate soulWells.
				for (auto& targetActor : targets)
				{
					ASoulWellActor* relatedSoulWell = nullptr;
					if (targetActor->GetClass()->IsChildOf(ASoulWellActor::StaticClass()))
					{
						relatedSoulWell = static_cast<ASoulWellActor*>(targetActor);
					} else if (targetActor->GetClass()->IsChildOf(ABattleArenaCharacter::StaticClass()))
					{
						ABattleArenaCharacter* character = static_cast<ABattleArenaCharacter*>(targetActor);
						relatedSoulWell = character->GetSpawnedSoulWell();
					}

					if (relatedSoulWell && relatedSoulWell->GetSoulWellTypeForPlayer(casterAllegiance) == ESoulWellType::ESWTeam0)
					{
						float threat = 0;
						int* threatPtr = soulWellThreatMap.Find(relatedSoulWell);
						if (threatPtr)
						{
							threat = *threatPtr;
						}
						else
						{
							threat = relatedSoulWell->CalculatePlayerThreat(casterAllegiance);
							soulWellThreatMap.Add(relatedSoulWell, threat);
						}

						if (threat > maxThreat)
						{
							maxThreatTargetIndex = index;
							threat = maxThreat;
						}
					}

					index++;
				}

				if (maxThreatTargetIndex >= 0)
				{
					return targets[maxThreatTargetIndex];
				}

			}

			// If target was not selected defensively, just select it random
			int randomTargetIndex = rand() % targets.Num();
			return targets[randomTargetIndex];
		}
	}

	return nullptr;
}