// <copyright file="SpawnQueueData.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SpawnQueueData.generated.h"


USTRUCT(BlueprintType)
/// <summary>
/// Data for the spawn queue
/// </summary>
struct SOULSHARDS_API FSpawnQueueData
{
	GENERATED_USTRUCT_BODY()
	
public:

	FSpawnQueueData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FSpawnQueueData)
	/// <summary>
	/// The actor to spawn
	/// </summary>
	AActor* Actor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FSpawnQueueData)
	/// <summary>
	/// The class of the actor to spawn
	/// </summary>
	UClass* Class;
};
