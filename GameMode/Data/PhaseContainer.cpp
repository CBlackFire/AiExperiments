// <copyright file="PhaseContainer.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>


#include "SoulShards.h"
#include "GameMode/GameInstance/SoulShardsGameInstance.h"
#include "PhaseContainer.h"

UPhaseContainer::UPhaseContainer()
{
	this->ContenDeckStartPhaseIndex = 0;
	this->IsSurvival = false;
	this->ForceReplayRecord = false;
}

TArray<UBattleArenaGamePhase*>& UPhaseContainer::GetPhaseArray()
{
	return this->PhasesArray;
}

TArray<UTriggeredGamePhase*>& UPhaseContainer::GetTriggerPhaseArray()
{
	return this->TriggerPhasesArray;
}

FString UPhaseContainer::GetContentDeckId() const
{
	return this->ContentDeckId;
}

int UPhaseContainer::GetContenDeckStartPhaseIndex() const
{
	return this->ContenDeckStartPhaseIndex;
}

FString UPhaseContainer::GetPlayerName() const
{
	return this->PlayerName;
}

bool UPhaseContainer::GetIsSurvival() const
{
	return this->IsSurvival;
}

const bool UPhaseContainer::GetForceReplayRecord()
{
	return this->ForceReplayRecord;
}