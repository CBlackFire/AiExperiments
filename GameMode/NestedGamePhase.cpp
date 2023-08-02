// <copyright file="NestedGamePhase.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "GameMode/BattleArenaGameMode.h"
#include "NestedGamePhase.h"

UNestedGamePhase::UNestedGamePhase()
{
	this->currentNestedPhaseIndex = 0;
}

UBattleArenaGamePhase* UNestedGamePhase::GetCurrentNestedPhase()
{
	int numPhases = this->NestedPhaseArray.Num();
	if (numPhases > 0 && this->currentNestedPhaseIndex < numPhases)
	{
		
		UBattleArenaGamePhase* currentPhase = this->NestedPhaseArray[this->currentNestedPhaseIndex];
		bool enabled = currentPhase->GetIsEnabled();

		// Looks for the next enabled phase.
		while (!enabled && currentPhase)
		{
			this->currentNestedPhaseIndex++;
			currentPhase = this->NestedPhaseArray[this->currentNestedPhaseIndex];
			if (currentPhase)
			{
				enabled = currentPhase->GetIsEnabled();
			}
		}

		return currentPhase;
	}
	return nullptr;
}

bool UNestedGamePhase::CheckPhaseEndCondition_Implementation()
{
	UBattleArenaGamePhase* currentPhase = this->GetCurrentNestedPhase();
	if (currentPhase)
	{
		int numPhases = this->NestedPhaseArray.Num();
		bool endCurrentPhase = currentPhase->CheckPhaseEndCondition();
		if (endCurrentPhase)
		{
			if (this->currentNestedPhaseIndex < numPhases)
			{
				ABattleArenaGameMode* gameMode = Cast<ABattleArenaGameMode>(this->GetWorld()->GetAuthGameMode());
				check(gameMode);
				TServerBattleArena* serverBattleArena = static_cast<TServerBattleArena*>(gameMode->GetBattleArena());
				currentPhase->EndPhase();

				this->currentNestedPhaseIndex++;

				currentPhase = this->GetCurrentNestedPhase();
				if (currentPhase)
				{
					currentPhase->StartPhase();
				}
			}
			else
			{
				return true;
			}
		}
	}
	else
	{
		return true;
	}

	return false;
}


bool UNestedGamePhase::CheckGameEndCondition_Implementation()
{
	UBattleArenaGamePhase* currentPhase = this->GetCurrentNestedPhase();
	if (currentPhase)
	{
		return currentPhase->CheckGameEndCondition();
	}

	return false;
}

void UNestedGamePhase::OnEventTriggered_Implementation(const FString& eventName, UObject* target)
{
	UBattleArenaGamePhase* currentPhase = this->GetCurrentNestedPhase();
	if (currentPhase)
	{
		return currentPhase->TriggerPhaseEvent(eventName, target);
	}
}

void UNestedGamePhase::StartPhase()
{
	Super::StartPhase();

	UBattleArenaGamePhase* firstPhase = this->GetCurrentNestedPhase();
	if (firstPhase)
	{
		firstPhase->StartPhase();
	}
}

void UNestedGamePhase::EndPhase()
{
	UBattleArenaGamePhase* currentPhase = this->GetCurrentNestedPhase();
	if (currentPhase)
	{
		return currentPhase->EndPhase();
	}

	Super::EndPhase();
}