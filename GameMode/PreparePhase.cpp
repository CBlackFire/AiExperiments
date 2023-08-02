// <copyright file="PreparePhase.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "GameMode/BattleArenaGameMode.h"
#include "Cosmetic/ClientTextNotificationEffect.h"
#include "PreparePhase.h"

UPreparePhase::UPreparePhase()
{
	this->SetPhaseName(TEXT("Preparation Phase"));
	this->SetPhaseGoalText(TEXT("Build your defenses"));
}

void UPreparePhase::StartPhase()
{
	Super::StartPhase();

	TServerBattleArena& serverBattleArena = *this->GetBattleArena();
	serverBattleArena.DisableSouWellAcquisitionPreparePhase();
	serverBattleArena.DisableConsume();
}

void UPreparePhase::EndPhase()
{
	TServerBattleArena& serverBattleArena = *this->GetBattleArena();
	serverBattleArena.EnableSouWellAcquisition();
	serverBattleArena.EnableConsume();

	Super::EndPhase();
}

/// <summary>
/// Returns the phase duration in seconds
/// </summary>
int UPreparePhase::GetPhaseDuration()
{
	return this->PreparationPhaseDuration;
}

UPreparePhase::~UPreparePhase()
{
}


