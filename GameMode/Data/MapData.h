// <copyright file="MapData.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsEnums.h"
#include "MapData.generated.h"


USTRUCT(BlueprintType)
/// <summary>
/// Data for a player
/// </summary>
struct SOULSHARDS_API FMapData 
{
	GENERATED_USTRUCT_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FMapData)
	/// <summary>
	/// Asset where the map is located
	/// </summary>
	FSoftObjectPath MapAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FMapData)
	/// <summary>
	/// Realm for this map
	/// </summary>
	EGameRealm MapRealm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FMapData)
	/// <summary>
	/// Whether this map is valid main menu
	/// </summary>
	bool ValidMainMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FMapData)
	/// <summary>
	/// Whether this map is a valid battle arena
	/// </summary>
	bool ValidBattleArena;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FMapData)
	/// <summary>
	/// Whether this map is valid for nightmare gameplay
	/// </summary>
	bool IsNightmareMap;

	/// <summary>
	/// Gets the namp name
	/// </summary>
	FString GetMapAssetName();
};
