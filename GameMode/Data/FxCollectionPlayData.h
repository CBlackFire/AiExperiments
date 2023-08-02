// <copyright file="FxCollectionPlayData.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "FxCollectionPlayData.generated.h"


USTRUCT(BlueprintType)
/// <summary>
/// Data used to play queued fx collections.
/// </summary>
struct SOULSHARDS_API FFxCollectionPlayData
{
	GENERATED_USTRUCT_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FxCollectionPlayData)
	/// <summary>
	/// Target actor that the fx collection will be played unto.
	/// </summary>
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FxCollectionPlayData)
	/// <summary>
	/// Class of the collection to be played.
	/// </summary>
	TSubclassOf<class AFxCollection> CollectionClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FxCollectionPlayData)
	/// <summary>
	/// Secondary targets for the collection.
	/// </summary>	
	TArray<AActor*> SecondaryTargetActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FxCollectionPlayData)
	/// <summary>
	/// Duration.
	/// </summary>	
	float Duration;

	FFxCollectionPlayData();
};
