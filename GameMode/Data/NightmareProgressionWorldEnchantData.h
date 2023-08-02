// <copyright file="NightmareProgressionWorldEnchantData.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "BattleEngine/D20/Casting/Spell.h"
#include "NightmareProgressionWorldEnchantData.generated.h"

USTRUCT(BlueprintType)
/// <summary>
/// Data for nightmare progression world enchant.
/// </summary>
struct SOULSHARDS_API FNightmareProgressionWorldEnchantData
{
	GENERATED_USTRUCT_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FNightmareWProgressionorldEnchantData)
	/// <summary>
	/// The spell class.
	/// </summary>	
	TSubclassOf<ASpell> SpellClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FNightmareProgressionWorldEnchantData)
	/// <summary>
	/// The start level
	/// </summary>	
	int StartLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FNightmareProgressionWorldEnchantData)
	/// <summary>
	/// The end level
	/// </summary>	
	int EndLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FNightmareProgressionWorldEnchantData)
	/// <summary>
	/// The party filter. The target of the spell should be of this type.
	/// </summary>	
	EPartyFilterType PartyFilter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FNightmareProgressionWorldEnchantData)
	/// <summary>
	/// The nightmare difficulty.
	/// </summary>	
	ENightmareDifficulty NightmareDifficulty;
};
