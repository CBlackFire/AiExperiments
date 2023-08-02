// <copyright file="TriggeredGamePhase.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/Scripted/ScriptedGamePhase.h"
#include "TriggeredGamePhase.generated.h"

UCLASS(BlueprintType, Blueprintable)
/// <summary>
/// Defines a triggered game phase tha can be extended in blueprints
/// </summary>
class SOULSHARDS_API UTriggeredGamePhase : public UScriptedGamePhase
{
	GENERATED_BODY()
	
private:


public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = UTriggeredGamePhase)
	/// <summary>
	/// Indicates if this phase should be triggered
	/// </summary>
	bool ShouldTrigger();	
};
