// <copyright file="SoulShardsGameMode.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>


#include "SoulShards.h"
#include "GameMode/GameInstance/SoulShardsGameInstance.h"
#include "GameMode/BattleArenaGameSession.h"
#include "BattleEngine/D20/Casting/Spell.h"
#include "Cosmetic/CharaterStatusMessageEffect.h"
#include "Cosmetic/ClientTextNotificationEffect.h"
#include "Cosmetic/CharacterParticlesCosmeticEffect.h"
#include "Character/SoulCaster/Actor/SoulShardsCharacter.h"
#include "Character/SoulCaster/Actor/SoulShardActor.h"
#include "GameMode/BattleArenaLevelScriptActor.h"
#include "SoulShardsHelper.hpp"
#include "SoulShardsGameMode.h"

ASoulShardsGameMode::ASoulShardsGameMode() : Super()
{
	UE_LOG(SoulShardsLog, Log, TEXT("ASoulShardsGameMode::ASoulShardsGameMode() started"));

	// set default pawn class to our Blueprinted character
	this->DefaultPawnClass = nullptr;

	this->DefaultCharacterParticlesCosmeticEffectClass = ACharacterParticlesCosmeticEffect::StaticClass();
	
#if !UE_EDITOR
	bool bUseAuthentication = true;
#endif    

	this->bUseSeamlessTravel = false;
}

TSubclassOf<AGameSession> ASoulShardsGameMode::GetGameSessionClass() const
{
	return ABattleArenaGameSession::StaticClass();
}

UClass* ASoulShardsGameMode::GetPlayerCharacterClass()
{
	return nullptr;
}

void ASoulShardsGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (this->GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(this->GetWorld()->GetGameInstance());
		gameInstance->GetClientGameInstanceManager()->EnsureQueuedWidgetDisplayed();
	}		
}

void ASoulShardsGameMode::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}

TSubclassOf<class ACharaterStatusMessageEffect> ASoulShardsGameMode::GetDefaultCharacterStatusMessageEffectClass()
{
	if (this->DefaultCharacterStatusMessageEffectClass.IsNull())
	{
		this->DefaultCharacterStatusMessageEffectClass =
            SoulShardsHelper::GetClassByString("/Game/Dev/GamePlay/ClientCosmeticActors/CCA_DefaultCharacterStatusMessageEffect.CCA_DefaultCharacterStatusMessageEffect_C", this); 
	}

	return this->DefaultCharacterStatusMessageEffectClass.LoadSynchronous();

}

TSubclassOf<class AClientTextNotificationEffect> ASoulShardsGameMode::GetDefaultCastNotificationEffectClass()
{
	if (this->DefaultCastNotificationEffectClass.IsNull())
	{
		this->DefaultCastNotificationEffectClass =
            SoulShardsHelper::GetClassByString("/Game/Dev/GamePlay/ClientCosmeticActors/CCA_DefaultCastNotificationEffect.CCA_DefaultCastNotificationEffect_C", this); 
	}

	return this->DefaultCastNotificationEffectClass.LoadSynchronous();
}

TSubclassOf<class AClientTextNotificationEffect> ASoulShardsGameMode::GetDefaultPhaseNotificationEffectClass()
{
	if (this->DefaultPhaseNotificationEffectClass.IsNull())
	{
		this->DefaultPhaseNotificationEffectClass =
            SoulShardsHelper::GetClassByString("/Game/Dev/GamePlay/ClientCosmeticActors/CCA_DefaultPhaseNotificationEffect.CCA_DefaultPhaseNotificationEffect_C", this); 
	}

	return this->DefaultPhaseNotificationEffectClass.LoadSynchronous();
}

TSubclassOf<class AClientTextNotificationEffect> ASoulShardsGameMode::GetDefaultUserWarningNotificationEffectClass()
{
	if (this->DefaultUserWarningNotificationEffectClass.IsNull())
	{
		this->DefaultUserWarningNotificationEffectClass =
            SoulShardsHelper::GetClassByString("/Game/Dev/GamePlay/ClientCosmeticActors/CCA_DefaultUserWarningNotificationEffect.CCA_DefaultUserWarningNotificationEffect_C", this); 
	}

	return this->DefaultUserWarningNotificationEffectClass.LoadSynchronous();
}

TSubclassOf<class ACharacterParticlesCosmeticEffect> ASoulShardsGameMode::GetDefaultCharacterParticlesCosmeticEffectClass()
{
	return this->DefaultCharacterParticlesCosmeticEffectClass.LoadSynchronous();
}


void ASoulShardsGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	this->CancelTimers(this->GetWorld());
	Super::EndPlay(EndPlayReason);
}

ABattleArenaLevelScriptActor* ASoulShardsGameMode::GetLevelActor()
{
	ABattleArenaLevelScriptActor* toReturn = Cast<ABattleArenaLevelScriptActor>(this->GetWorld()->GetLevelScriptActor());
	checkf(toReturn, TEXT("Level actor is not of the ABattleArenaLevelScriptActor type"));
	return toReturn;
}

bool ASoulShardsGameMode::ShouldRecordReplay()
{
	return false;
}