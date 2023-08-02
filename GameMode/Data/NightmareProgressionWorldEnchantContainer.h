// <copyright file="NightmareProgressionWorldEnchantContainer.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsEnums.h"
#include "NightmareProgressionWorldEnchantData.h"
#include "NightmareProgressionWorldEnchantContainer.generated.h"

UCLASS(Blueprintable, BlueprintType)
/// <summary>
/// Class that contains information about nightmare progression world enchants.
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API UNightmareProgressionWorldEnchantContainer : public UObject
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UNightmareProgressionWorldEnchantContainer, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Array with all thenightmare progression world enchants.
	/// </summary>
	TArray<FNightmareProgressionWorldEnchantData> NightmareProgressionWorldEnchantArray;

public:

	/// <summary>
	/// Gets the nightmare progression world enchants by level.
	/// </summary>
	/// <param name="level">The game level.</param>
	/// <returns>The nightmare progression world enchants.</returns>
	TArray<FNightmareProgressionWorldEnchantData> GetNightmareProgressionWorldEnchants(int level);

	UFUNCTION(BlueprintCallable, Category = UNightmareProgressionWorldEnchantContainer)
	/// <summary>
	/// Gets the next nightmare progression world enchants by level.
	/// </summary>
	/// <param name="level">The game level.</param>
	/// <returns>The nightmare progression world enchants.</returns>
	TArray<FNightmareProgressionWorldEnchantData> GetNextNightmareProgressionWorldEnchants(int level);

	UFUNCTION(BlueprintCallable, Category = UNightmareProgressionWorldEnchantContainer)
	/// <summary>
	/// Gets the nightmare progression world enchants by level and party filter.
	/// </summary>
	/// <param name="level">The game level.</param>
	/// <param name="partyFilter">The party filter.</param>
	/// <returns>The nightmare progression world enchants.</returns>
	TArray<FNightmareProgressionWorldEnchantData> GetNightmareProgressionWorldEnchants(int level, EPartyFilterType partyFilter);
};
