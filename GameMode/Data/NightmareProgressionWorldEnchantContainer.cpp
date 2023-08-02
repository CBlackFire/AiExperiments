// <copyright file="NightmareProgressionWorldEnchantContainer.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>


#include "SoulShards.h"
#include "NightmareProgressionWorldEnchantContainer.h"

TArray<FNightmareProgressionWorldEnchantData> UNightmareProgressionWorldEnchantContainer::GetNightmareProgressionWorldEnchants(int level)
{
	TArray<FNightmareProgressionWorldEnchantData> result;
	for (auto& nightmareProgressionWorldEnchantData : this->NightmareProgressionWorldEnchantArray)
	{
		if ((level >= nightmareProgressionWorldEnchantData.StartLevel) && (level <= nightmareProgressionWorldEnchantData.EndLevel))
		{
			result.Add(nightmareProgressionWorldEnchantData);
		}
	}

	return result;
}

TArray<FNightmareProgressionWorldEnchantData> UNightmareProgressionWorldEnchantContainer::GetNextNightmareProgressionWorldEnchants(int level)
{
	TArray<FNightmareProgressionWorldEnchantData> result;
	for (auto& nightmareProgressionWorldEnchantData : this->NightmareProgressionWorldEnchantArray)
	{
		if (level < nightmareProgressionWorldEnchantData.StartLevel)
		{
			result.Add(nightmareProgressionWorldEnchantData);
			break;
		}
	}

	return result;
}

TArray<FNightmareProgressionWorldEnchantData> UNightmareProgressionWorldEnchantContainer::GetNightmareProgressionWorldEnchants(int level, EPartyFilterType partyFilter)
{
	TArray<FNightmareProgressionWorldEnchantData> result;
	for (auto& nightmareProgressionWorldEnchantData : this->NightmareProgressionWorldEnchantArray)
	{
		if ((level >= nightmareProgressionWorldEnchantData.StartLevel) &&
			(level <= nightmareProgressionWorldEnchantData.EndLevel) &&
			((partyFilter == EPartyFilterType::PFTAny) || (partyFilter == nightmareProgressionWorldEnchantData.PartyFilter)))
		{
			result.Add(nightmareProgressionWorldEnchantData);
		}
	}

	return result;
}
