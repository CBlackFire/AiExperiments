// <copyright file="PhaseEventData.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "GameMode/BattleArenaGamePhase.h"
#include "PhaseEventData.h"

FPhaseEventData::FPhaseEventData(): GamePhase(nullptr), Target(nullptr)
{
}

FPhaseEventData::FPhaseEventData(UBattleArenaGamePhase* gamePhase, UObject* target, FString eventName)
{
	this->GamePhase = gamePhase;
	this->Target = target;
	this->EventName = eventName;
}

