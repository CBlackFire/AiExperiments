// <copyright file="SoulShardsGameMode.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "CoreMinimal.h"
#include "CancelableTimerProviderInterface.h"
#include "GameFramework/GameMode.h"
#include "SoulShardsGameMode.generated.h"

UCLASS(minimalapi)
/// <summary>
/// Base game mode that provides menu capabilities
/// </summary>
/// <seealso cref="AGameMode" />
class ASoulShardsGameMode : public AGameMode, public ICancelableTimerProviderInterface
{
	GENERATED_BODY()

private:

	UPROPERTY()
	/// <summary>
	/// Default class used to spawn status messages
	/// </summary>
	TSoftClassPtr<class ACharaterStatusMessageEffect> DefaultCharacterStatusMessageEffectClass;

	UPROPERTY()
	/// <summary>
	/// Default class used to spawn cast notifications
	/// </summary>
	TSoftClassPtr<class AClientTextNotificationEffect> DefaultCastNotificationEffectClass;

	UPROPERTY()
	/// <summary>
	/// Default class used to spawn phase notifications
	/// </summary>
	TSoftClassPtr<class AClientTextNotificationEffect> DefaultPhaseNotificationEffectClass;

	UPROPERTY()
	/// <summary>
	/// Default class used to spawn user warning notifications
	/// </summary>
	TSoftClassPtr<class AClientTextNotificationEffect> DefaultUserWarningNotificationEffectClass;

	UPROPERTY()
	/// <summary>
	/// Default class used to spawn character particles
	/// </summary>
	TSoftClassPtr<class ACharacterParticlesCosmeticEffect> DefaultCharacterParticlesCosmeticEffectClass;

protected:

	/// <summary>
	/// Gets the game session class.
	/// </summary>
	/// <returns></returns>
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;

	/// <summary>
	/// Called every frame
	/// </summary>
	/// <param name="deltaTime">The delta time.</param>
	virtual void Tick(float deltaTime) override;

	/// <summary>
	/// Called when the game starts
	/// </summary>
	virtual void BeginPlay() override;

	/// <summary>
	/// Called when it is decided that the object will be destroyed
	/// </summary>
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="ASoulShardsGameMode"/> class.
	/// </summary>
	ASoulShardsGameMode();	

	/// <summary>
	/// Gets iterator for a type of actor.
	/// </summary>
	/// <returns></returns>
	template<class T> TActorIterator<T> GetActorIterator()
	{
		return TActorIterator<T>(this->GetWorld());
	}

	/// <summary>
	/// Gets iterator for a type of actor.
	/// </summary>
	/// <returns></returns>
	template<class T> TActorIterator<T> GetActorIterator(TSubclassOf<T> rootClass)
	{
		return TActorIterator<T>(this->GetWorld(), rootClass);
	}

	/// <summary>
	/// Gets the player character class.
	/// </summary>
	/// <returns></returns>
	virtual UClass* GetPlayerCharacterClass();

	/// <summary>
	/// Gets the default character status message effect class.
	/// </summary>
	TSubclassOf<class ACharaterStatusMessageEffect> GetDefaultCharacterStatusMessageEffectClass();

	/// <summary>
	/// Gets the default cast notification effect class.
	/// </summary>
	TSubclassOf<class AClientTextNotificationEffect> GetDefaultCastNotificationEffectClass();

	/// <summary>
	/// Gets the default phase notification effect class.
	/// </summary>
	TSubclassOf<class AClientTextNotificationEffect> GetDefaultPhaseNotificationEffectClass();

	/// <summary>
	/// Gets the default user warning effect class.
	/// </summary>
	TSubclassOf<class AClientTextNotificationEffect> GetDefaultUserWarningNotificationEffectClass();

	/// <summary>
	/// Gets the default class used to spawn character particles
	/// </summary>
	TSubclassOf<class ACharacterParticlesCosmeticEffect> GetDefaultCharacterParticlesCosmeticEffectClass();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = ASoulShardsGameMode)
	/// <summary>
	/// Gets the level actor
	/// </summary>
	class ABattleArenaLevelScriptActor* GetLevelActor();

	/// <summary>
	/// Should be overriden by base classes to indicate if the game mode wants the game to be recorded.
	/// </summary>
	/// <returns>
	/// False.
	/// </returns>
	virtual bool ShouldRecordReplay();
};



