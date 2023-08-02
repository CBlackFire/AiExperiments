// <copyright file="CameraScriptContainer.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsEnums.h"
#include "Player/Data/ReplayCameraScriptStepsData.h"
#include "CameraScriptContainer.generated.h"

UCLASS(Blueprintable, BlueprintType)
/// <summary>
/// Class that contains information about camera scripts
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API UCameraScriptContainer : public UObject
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UCameraScriptContainer, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Array with all the camera scripts
	/// </summary>
	TMap<FString, FReplayCameraScriptStepsData> ScriptsMap;

public:

	FReplayCameraScriptStepsData* GetScriptByName(FString scriptName);
};