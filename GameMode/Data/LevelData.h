// <copyright file="LevelData.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsEnums.h"
#include "MinimapData.h"
#include "LevelData.generated.h"

UCLASS(Blueprintable, BlueprintType)
/// <summary>
/// Class that contains information about a level.
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API ULevelData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Name for the level.
	/// </summary>
	FText LevelName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// The D20 to world unit conversion
	/// </summary>
	float BattleEngineToWorldUnitConversion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// The terrain size
	/// </summary>
	float TerrainSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// The maximum fly height
	/// </summary>
	float MaxFlyHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// The soul well radius
	/// </summary>
	float SoulWellRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// The totem zone radius
	/// </summary>
	float TotemZoneRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// The maximun number of players
	/// </summary>
	float MaxPlayerCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Flag indicating whether automated a replay should be recorded for this game.
	/// </summary>
	bool ShouldRecordReplay;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Number of times level intro must be played before moving forward.
	/// </summary>
	int MinLoopLevelIntroIterationCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Battle deck lights intensity.
	/// </summary>
	float BattleDeckLightsIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// The Minimap information.
	/// </summary>
	FMinimapData MinimapData;

	/// <summary>
	/// Initializes a new instance of the UDeckContainer type. 
	/// </summary>
	ULevelData();

	UFUNCTION(BlueprintCallable, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Gets the minimap background image.
	/// </summary>
	FSlateBrush GetMinimapBackgroundImage() const;
};
