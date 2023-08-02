// <copyright file="NightmareProgressionChallengeContainer.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "NightmareProgressionChallengeData.h"
#include "NightmareProgressionChallengeContainer.generated.h"

UCLASS(Blueprintable, BlueprintType)
/// <summary>
/// Class that contains information about nightmare progression challenges
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API UNightmareProgressionChallengeContainer : public UObject
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UNightmareProgressionChallengeContainer, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Array with all the nightmare progression challenges
	/// </summary>
	TArray<FNightmareProgressionChallengeData> NightmareProgressionChallengeArray;

public:

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = UNightmareProgressionChallengeContainer)
	/// <summary>
	/// Gets a nightmare progression challenge.
	/// </summary>
	/// <param name="level">The game level.</param>
	/// <returns>The challenge ID</returns>
	FString GetNightmareProgressionChallengeID(int level);
};
