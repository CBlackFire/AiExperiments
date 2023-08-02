// <copyright file="MapContainer.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsEnums.h"
#include "GameMode/Data/MapData.h"
#include "MapContainer.generated.h"

UCLASS(Blueprintable, BlueprintType)
/// <summary>
/// Class that contains information about playable maps
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API UMapContainer : public UObject
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UMapContainer, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Array with all the game maps
	/// </summary>
	TArray<FMapData> mapArray;

	/// <summary>
	/// Selects the random map.
	/// </summary>
	/// <param name="mapsToSelect">The maps to select.</param>
	/// <returns></returns>
	FMapData SelectRandomMap(TArray<FMapData> mapsToSelect);

public:

	/// <summary>
	/// Gets the start map.
	/// </summary>
	/// <returns></returns>
	FMapData GetStartMap();

	/// <summary>
	/// Gets a main menu map.
	/// </summary>
	/// <param name="gameRealm">The game realm.</param>
	/// <returns></returns>
	FMapData GetMainMenuMap(EGameRealm gameRealm = EGameRealm::EGRUknown);

	/// <summary>
	/// Gets a battle arena map.
	/// </summary>
	/// <param name="gameRealm">The game realm.</param>
	/// <returns></returns>
	FMapData GetBattleArenaMap(EGameRealm gameRealm = EGameRealm::EGRUknown);

	/// <summary>
	/// Gets a nightmare battle arena map.
	/// </summary>
	/// <param name="gameRealm">The game realm.</param>
	/// <returns></returns>
	FMapData GetNightmareMap(EGameRealm gameRealm = EGameRealm::EGRUknown);

};
