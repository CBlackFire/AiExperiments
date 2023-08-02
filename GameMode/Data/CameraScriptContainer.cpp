// <copyright file="CameraScriptContainer.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>


#include "SoulShards.h"
#include "CameraScriptContainer.h"

FReplayCameraScriptStepsData* UCameraScriptContainer::GetScriptByName(FString scriptName)
{
	return this->ScriptsMap.Find(scriptName);
}