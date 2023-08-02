// <copyright file="DuelGameMode.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "Player/Controller/BattleArenaPlayerController.h"
#include "GameMode/DuelGameMainPhase.h"
#include "GameMode/PreparePhase.h"
#include "DuelGameMode.h"

ADuelGameMode::ADuelGameMode()
{
	this->PlayerControllerClass = ABattleArenaPlayerController::StaticClass();
}

void ADuelGameMode::InitPhases(FGameOptionsData contentId)
{
	UPreparePhase* preparePhase = NewObject<UPreparePhase>(this, UPreparePhase::StaticClass(), TEXT("DuelGamePreparePhase"));
	UDuelGameMainPhase* mainPhase = NewObject<UDuelGameMainPhase>(this, UDuelGameMainPhase::StaticClass(), TEXT("DuelGameMainPhase"));

	this->AddPhase(preparePhase);
	this->AddPhase(mainPhase);
}

int ADuelGameMode::GetMinPlayersToStartMatch()
{
	return 2;
}

bool ADuelGameMode::ShouldRecordReplay()
{
	return true;
}
