// <copyright file="TournamentManager.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>


#include "SoulShards.h"
#include "TournamentManager.h"


UTournamentManager::UTournamentManager()
{
	this->parentGameInstance = nullptr;
}


UWorld* UTournamentManager::GetWorld() const
{
	if (this->parentGameInstance)
	{
		return this->parentGameInstance->GetWorld();
	}

	return nullptr;
}

UGameInstance* UTournamentManager::GetParentGameInstance()
{
	return this->parentGameInstance;
}

void UTournamentManager::SetParentGameInstance(UGameInstance* gameInstance)
{
	this->parentGameInstance = gameInstance;
}
