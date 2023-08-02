// <copyright file="SoulShardsBaseLevelScriptActor.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "SoulShardsBaseLevelScriptActor.h"

#include "SoulShardsHelper.hpp"
#include "Player/Controller/BattleArenaPlayerController.h"

ASoulShardsBaseLevelScriptActor::ASoulShardsBaseLevelScriptActor()
{
	// Some operations should only be initialized when the game instance is the real deal, not CDO or Sekl
	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		// Hook up to get notifications so we know when a travel is complete (LoadMap or Seamless).
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnPostLoadMapWithWorld);
	}

	PrimaryActorTick.bCanEverTick = true;
}

void ASoulShardsBaseLevelScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	this->CancelTimers(this->GetWorld());
	Super::EndPlay(EndPlayReason);
}


void ASoulShardsBaseLevelScriptActor::OnPostLoadMapWithWorld(UWorld* world) const
{
}

void ASoulShardsBaseLevelScriptActor::QueuePlayFxCollection(
	AActor* target, 
	TSubclassOf<AFxCollection> collectionClass, 	
	const TArray<AActor*>& secondaryTargetActors,
	float duration)
{
	FFxCollectionPlayData playData;
	playData.Target = target;
	playData.CollectionClass = collectionClass;
	playData.SecondaryTargetActors = secondaryTargetActors;
	playData.Duration = duration;
	
	this->queuedFxCollections.Add(playData);	
}

void ASoulShardsBaseLevelScriptActor::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	for (auto queuedFxCollection: this->queuedFxCollections)
	{
		if (queuedFxCollection.Target->IsValidLowLevel())
		{
			AFxCollection::PlayFxCollection2(
			queuedFxCollection.Target,
			queuedFxCollection.CollectionClass,
			queuedFxCollection.SecondaryTargetActors,
			queuedFxCollection.Duration);
		}
	}
	
	this->queuedFxCollections.Reset();
}