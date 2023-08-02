// <copyright file="MinimapData.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#define LOCTEXT_NAMESPACE "A3Nodes"

#include "SoulShards.h"
#include "MinimapData.h"

#define LOCTEXT_NAMESPACE "SoulShards"

FMinimapData::FMinimapData()
{
	this->BackgroundTranslation = FVector2D::Zero();
	this->BackgroundScale = FVector2D::One();
	this->WorldToWidgetScaleMultiplier = FVector2D::One();
	this->WorldToWidgetOffsetMultiplier = FVector2D::One();
}