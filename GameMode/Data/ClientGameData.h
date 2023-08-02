// <copyright file="ClientGameData.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "Player/Data/DeckData.h"
#include "Player/Data/GameOptionsData.h"
#include "Player/Data/FriendData.h"
#include "ClientGameData.generated.h"


USTRUCT(BlueprintType)
/// <summary>
/// Data for a player
/// </summary>
struct SOULSHARDS_API FClientGameData 
{
	GENERATED_USTRUCT_BODY()
	
public:

	FClientGameData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FClientGameData)
	/// <summary>
	/// The game options
	/// </summary>
	FGameOptionsData GameOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FClientGameData)
	/// <summary>
	/// Value determining whether the current player will attempt to create the game
	/// </summary>
	bool IsGameCreator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FClientGameData)
	/// <summary>
	/// Friend data containing the information of the friend to be challenged in the case this is challenge a friend.
	/// </summary>
	FFriendData FriendData;
};
