// <copyright file="MapContainer.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>


#include "SoulShards.h"
#include "GameMode/GameInstance/SoulShardsGameInstance.h"
#include "MapContainer.h"

FMapData UMapContainer::SelectRandomMap(TArray<FMapData> mapsToSelect)
{
	int mapCount = mapsToSelect.Num();
	check(mapCount > 0);
	int randomMap = rand() % mapCount;
	return mapsToSelect[randomMap];
}

FMapData UMapContainer::GetStartMap()
{
	FMapData result;
	for (auto& mapData : this->mapArray)
	{
		if (!mapData.ValidMainMenu && !mapData.IsNightmareMap && !mapData.ValidBattleArena)
		{
			result = mapData;
			break;
		}
	}

	return result;
}

FMapData UMapContainer::GetMainMenuMap(EGameRealm gameRealm)
{
	TArray<FMapData> filteredMaps;
	for (auto& mapData : this->mapArray)
	{
		if (mapData.ValidMainMenu && ((gameRealm == EGameRealm::EGRUknown) || (mapData.MapRealm == gameRealm)))
		{
			filteredMaps.Add(mapData);
		}
	}

	return this->SelectRandomMap(filteredMaps);
}

FMapData UMapContainer::GetBattleArenaMap(EGameRealm gameRealm)
{
	TArray<FMapData> filteredMaps;
	for (auto& mapData : this->mapArray)
	{
		if (mapData.ValidBattleArena && ((gameRealm == EGameRealm::EGRUknown) || (mapData.MapRealm == gameRealm)))
		{
			// When game realm is unknown, pick any non-tutorial map.  Tutorial maps will need to be specifically selected
			if ((gameRealm == EGameRealm::EGRUknown))
			{
				filteredMaps.Add(mapData);
			}
			else if (mapData.MapRealm == gameRealm)
			{
				filteredMaps.Add(mapData);
			}
		}
	}

	return this->SelectRandomMap(filteredMaps);
}

FMapData UMapContainer::GetNightmareMap(EGameRealm gameRealm)
{
	TArray<FMapData> filteredMaps;
	for (auto& mapData : this->mapArray)
	{
		if (mapData.IsNightmareMap && ((gameRealm == EGameRealm::EGRUknown) || (mapData.MapRealm == gameRealm)))
		{
			filteredMaps.Add(mapData);
		}
	}

	return this->SelectRandomMap(filteredMaps);
}
