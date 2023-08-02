// <copyright file="TimeLimitGamePhase.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "GameMode/BattleArenaGameMode.h"
#include "Cosmetic/ClientTextNotificationEffect.h"
#include "TimeLimitGamePhase.h"

UTimeLimitGamePhase::UTimeLimitGamePhase()
{
	this->EnablePhaseNotification = true;
	this->ShowTimer = true;
}

bool UTimeLimitGamePhase::CheckGameEndCondition_Implementation()
{
	return false;
}

bool UTimeLimitGamePhase::CheckPhaseEndCondition_Implementation()
{
	int phaseDuration = this->GetPhaseDuration();

	// A phase duration of -1 is considered infinite
	if (phaseDuration >= 0)
	{
		ABattleArenaGameMode* gameMode = Cast<ABattleArenaGameMode>(this->GetWorld()->GetAuthGameMode());
		if (gameMode)
		{
			TServerBattleArena* serverBattleArena = static_cast<TServerBattleArena*>(gameMode->GetBattleArena());
			float currentTime = serverBattleArena->GetTime();
			if (currentTime - this->phaseStartTime > this->GetPhaseDuration())
			{
				return true;
			}
		}
	}

	return false;
}

void UTimeLimitGamePhase::StartPhase()
{
	Super::StartPhase();

	int phaseDuration = this->GetPhaseDuration();

	TServerBattleArena& serverBattleArena = *this->GetBattleArena();
	this->phaseStartTime = serverBattleArena.GetTime();

	// Inifinite phases wont display a timer
	if (phaseDuration >= 0)
	{
		FString phaseName;
		FString phaseGoal;
		if (EnablePhaseNotification)
		{
			phaseName = this->GetPhaseName();
			phaseGoal = this->GetPhaseGoalText();

			const int durationSeconds = this->GetPhaseDuration();
			const FString minutesStr = SoulShardsBaseHelper::GetTimerString(durationSeconds);
			phaseGoal = phaseGoal.Replace(ANSI_TO_TCHAR("{time}"), *minutesStr, ESearchCase::IgnoreCase);
		}
		
		//TODO: Localization
		//Generates the visual effect notification on the client side
		AClientTextNotificationEffect::SpawnPhaseNotification(
			this->GetWorld(),
			phaseName,
			phaseGoal,
			this->ShowTimer,
			static_cast<ABattleArenaGameMode*>(serverBattleArena.GetGameMode())->GetDefaultPhaseNotificationEffectClass(),
			phaseDuration);
	}

	//TSoundTrackEffect soundTrack(battleArena, "Music/Intro", nullptr, FVector(0.f, 0.f, 0.f));
	//soundTrack.ApplyEffect();
}

void UTimeLimitGamePhase::EndPhase()
{
	Super::EndPhase();
}

int UTimeLimitGamePhase::GetPhaseDuration()
{
	return 0;
}

UTimeLimitGamePhase::~UTimeLimitGamePhase()
{
}