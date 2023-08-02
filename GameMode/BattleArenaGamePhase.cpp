// <copyright file="BattleArenaGamePhase.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "BattleArenaGamePhase.h"
#include "GameMode/BattleArenaGameMode.h"

UBattleArenaGamePhase::UBattleArenaGamePhase()
{
	this->phaseIndex = 0;
	this->Enabled = true;
	this->AllowSummonQueueing = true;
	this->AllowCreatureDrops = true;
}

bool UBattleArenaGamePhase::CheckPhaseEndCondition_Implementation()
{
	return false;
}


bool UBattleArenaGamePhase::CheckGameEndCondition_Implementation()
{
	return false;
}

void UBattleArenaGamePhase::StartPhase()
{
	// Make sure crosshair is always enabled for battle phases.
	ABattleArenaGameMode* gameMode = Cast<ABattleArenaGameMode>(this->GetWorld()->GetAuthGameMode());
	gameMode->SetGameLevelFeatureEnabled(EConfigurableGameLevelFeature::CGFHudCrossHair, true);
	gameMode->SetGameLevelFeatureEnabled(EConfigurableGameLevelFeature::CGFCreatureDamage, true);

	this->OnPhaseStart();
}

void UBattleArenaGamePhase::OnPhaseStart_Implementation()
{	
}

void UBattleArenaGamePhase::EndPhase()
{
	this->OnPhaseEnd();
}

UWorld* UBattleArenaGamePhase::GetWorld() const
{
	// The CDO doesn't have a World
	return HasAnyFlags(RF_ClassDefaultObject) ? nullptr : this->GetOuter()->GetWorld();
}

FString UBattleArenaGamePhase::GetPhaseName() const
{
	return this->PhaseName;
}

FString UBattleArenaGamePhase::GetPhaseGoalText() const
{
	return this->PhaseGoalText;
}

void UBattleArenaGamePhase::SetPhaseName(FString phaseName)
{
	this->PhaseName = phaseName;
}

void UBattleArenaGamePhase::SetPhaseGoalText(FString phaseGoalText)
{
	this->PhaseGoalText = phaseGoalText;
}

int UBattleArenaGamePhase::GetPhaseIndex() const
{
	return this->phaseIndex;
}

bool UBattleArenaGamePhase::GetIsEnabled() const
{
	return this->Enabled;
}

void UBattleArenaGamePhase::OnEventTriggered_Implementation(const FString& eventName, UObject* target)
{

}

void UBattleArenaGamePhase::TriggerPhaseEvent(const FString& eventName, UObject* target)
{
	this->OnEventTriggered(eventName, target);
}


void UBattleArenaGamePhase::SetPhaseIndex(int phaseIndex)
{
	this->phaseIndex = phaseIndex;
}

TServerBattleArena* UBattleArenaGamePhase::GetBattleArena()
{
	ABattleArenaGameMode* gameMode = Cast<ABattleArenaGameMode>(this->GetWorld()->GetAuthGameMode());
	if (gameMode)
	{
		return static_cast<TServerBattleArena*>(gameMode->GetBattleArena());
	}

	return nullptr;
}

bool UBattleArenaGamePhase::GetAllowSummonQueueing()
{
	return this->AllowSummonQueueing;
}

bool UBattleArenaGamePhase::GetAllowCreatureDrops()
{
	return this->AllowCreatureDrops;
}

