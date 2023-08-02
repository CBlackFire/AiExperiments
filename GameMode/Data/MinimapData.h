// <copyright file="MinimapData.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsEnums.h"
#include "MinimapData.generated.h"

USTRUCT(BlueprintType)
/// <summary>
/// Class that contains information about a level minimap.
/// </summary>
struct SOULSHARDS_API FMinimapData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// The minimap background translation.
	/// </summary>
	FVector2D BackgroundTranslation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// The minimap background scale.
	/// </summary>
	FVector2D BackgroundScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// The world to widget scale multiplier.
	/// </summary>
	FVector2D WorldToWidgetScaleMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// The world to widget offset multiplier.
	/// </summary>
	FVector2D WorldToWidgetOffsetMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// The Minimap background image.
	/// </summary>
	FSoftObjectPath BackgroundImageReference;

	/// <summary>
	/// Initializes a new instance of the UDeckContainer type. 
	/// </summary>
	FMinimapData();
};
