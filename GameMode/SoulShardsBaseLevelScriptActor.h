// <copyright file="SoulShardsBaseLevelScriptActor.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "Engine/LevelScriptActor.h"
#include "Player/Data/CameraTransitionData.h"
#include "CancelableTimerProviderInterface.h"
#include "Data/FxCollectionPlayData.h"
#include "SoulShardsBaseLevelScriptActor.generated.h"

UCLASS(Config = Game, Blueprintable, BlueprintType)
/// <summary>
/// Class that represents a main menu level
/// </summary>
/// <seealso cref="ALevelScriptActor" />
class SOULSHARDS_API ASoulShardsBaseLevelScriptActor : public ALevelScriptActor, public ICancelableTimerProviderInterface
{
	GENERATED_BODY()
private:

	/// <summary>
	/// Queued fx collections.
	/// </summary>	
	TArray<FFxCollectionPlayData> queuedFxCollections;

protected:

	/// <summary>
	/// Called just after the map finishes loading.
	/// </summary>
	virtual void OnPostLoadMapWithWorld(UWorld* World) const;

	/// <summary>
	/// Actor Tick
	/// </summary>
	/// <param name="DeltaTime">The delta time.</param>
	virtual void Tick(float DeltaTime) override;	
	
public:

	/// <summary>
	/// Creates a new instance of <see cref="ASoulShardsBaseLevelScriptActor"/> class
	/// </summary>
	ASoulShardsBaseLevelScriptActor();


	UFUNCTION(BlueprintImplementableEvent, Category = UBattleArenaGamePhase)
	/// <summary>
	/// Called when there's a camera transition in the level.
	/// </summary>
	void TriggerCameraTransitionSound(const FCameraTransitionData& transitionData);

	UFUNCTION(BlueprintImplementableEvent, Category = UBattleArenaGamePhase)
	/// <summary>
	/// Called when the view camera changes.
	/// </summary>
	void OnViewCameraChanged(FName cameraName, AActor* cameraActor);	

	/// <summary>
	/// Called when it is decided that the object will be destroyed
	/// </summary>
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/// <summary>
	/// Queue the Playing of the Fx collection for the next frame (if supported).
	/// </summary>
	/// <param name="duration">Duration will be passed to fx items in case they can handle it.</param>
	void QueuePlayFxCollection(
		AActor* target,
		TSubclassOf<AFxCollection> collectionClass,
		const TArray<AActor*>& secondaryTargetActors,
		float duration = -1.0f);
};
