// <copyright file="LevelData.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#define LOCTEXT_NAMESPACE "A3Nodes"

#include "SoulShards.h"
#include "LevelData.h"

#define LOCTEXT_NAMESPACE "SoulShards"

ULevelData::ULevelData()
{
	this->BattleEngineToWorldUnitConversion = 30.48;
	this->TerrainSize = 35000;
	this->MaxFlyHeight = 500;
	this->SoulWellRadius = 150;
	this->TotemZoneRadius = 2500;
	this->MaxPlayerCount = 2;	
	this->ShouldRecordReplay = true;
	this->MinLoopLevelIntroIterationCount = 6;
	this->LevelName = LOCTEXT("LevelName", "Unnamed");
	this->BattleDeckLightsIntensity = 0.0f;
}

FSlateBrush ULevelData::GetMinimapBackgroundImage() const
{
	return SoulShardsBaseHelper::GetBrushFromTextureReference(this->MinimapData.BackgroundImageReference);
}
