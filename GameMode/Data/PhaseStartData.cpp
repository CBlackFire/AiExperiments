// <copyright file="PhaseStartData.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "PhaseStartData.h"

FPhaseStartData::FPhaseStartData()
{
	this->StartCameraTag = FName("Caster");
	this->RequiresManaToCast = false;
	this->ApplyInputMask = false;
	this->ApplyInputMask2 = false;
	this->InputMask = EIMAllActions;
	this->InputMask2 = EIMAllActions2;
	this->ApplyAllowedSpells = false;
	this->ApplyProhibitedSpells = false;
	this->ShuffleAllIntoLibrary = false;
	this->InputMask = (int)EIMAllActions;
	this->HealPlayer0 = false;
	this->DestroyAllCreatures = false;
	this->ReturnRemovedCardsToLibrary = false;
	this->ApplyAutoAdvanceConfiguration = false;
	this->ApplyReminder = true;
	this->EnableAutoDefenseDome = true;
	this->EnableHudCharacterUpdates = true;
	this->EnableCreatureMigration = true;
	this->IgnorePhaseData = false;
	this->EnableCrossHair = true;
	this->EnableCreatureDamage = true;
	AutoAdvanceConfiguration.Add(true);
	AutoAdvanceConfiguration.Add(true);
	AutoAdvanceConfiguration.Add(true);
}