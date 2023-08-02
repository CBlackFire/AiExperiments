// <copyright file="NightmareProgressionChallengeData.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "NightmareProgressionChallengeData.generated.h"

USTRUCT(BlueprintType)
/// <summary>
/// Data for nightmare progression challenge
/// </summary>
struct SOULSHARDS_API FNightmareProgressionChallengeData
{
	GENERATED_USTRUCT_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FNightmareProgressionChallengeData)
	/// <summary>
	/// Deck identifier
	/// </summary>	
	FString DeckId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FNightmareProgressionChallengeData)
	/// <summary>
	/// The nightmare level
	/// </summary>	
	int Level;
};
