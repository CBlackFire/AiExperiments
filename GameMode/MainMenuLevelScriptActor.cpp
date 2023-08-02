// <copyright file="MainMenuLevelScriptActor.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "AutoSettings/Public/SettingsManager.h"
#include "GameMode/BattleArenaGameMode.h"
#include "Character/SoulCaster/Actor/SoulShardsCharacter.h"
#include "Player/Controller/PlayerInputController.h"
#include "MainMenuLevelScriptActor.h"

AMainMenuLevelScriptActor::AMainMenuLevelScriptActor()
{
}

void AMainMenuLevelScriptActor::OnPostLoadMapWithWorld(UWorld* world) const
{
	this->SetViewCameraToStart(world);		
}

void AMainMenuLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	USettingsManager::RegisterStringCVarSetting(FName("HomeDefense.Deck"), FString(), FString());
	USettingsManager::RegisterStringCVarSetting(FName("Nightmare.Deck"), FString(), FString());
	USettingsManager::RegisterStringCVarSetting(FName("Multiplayer.Deck"), FString(), FString());
		
	USettingsManager::RegisterStringCVarSetting(FName("Nightmare.Difficulty"), FString("2"), FString());
	USettingsManager::RegisterStringCVarSetting(FName("HomeDefense.Difficulty"), FString("2"), FString());

	USettingsManager::RegisterStringCVarSetting(FName("WelcomeMessageShown"), FString("0"), FString());

	USettingsManager::RegisterFloatCVarSetting(FName("GameControls.InvertYAxis"), 0.0f, FString());
	USettingsManager::RegisterFloatCVarSetting(FName("GameControls.Sensitivity"), 0.5f, FString());

	USettingsManager::RegisterStringCVarSetting(FName("HDBuilder.HelpShown"), FString("0"), FString());

	USettingsManager::RegisterStringCVarSetting(FName("HUD.ShowMinimap"), FString("1"), FString());
	USettingsManager::RegisterStringCVarSetting(FName("HUD.ShowPlayedCards"), FString("1"), FString());
	USettingsManager::RegisterStringCVarSetting(FName("HUD.ShowEnemyPlayedCards"), FString("1"), FString());
	USettingsManager::RegisterStringCVarSetting(FName("HUD.ShowSelectedCard"), FString("1"), FString());	
}

void AMainMenuLevelScriptActor::SetViewCameraToStart(UWorld* world) const
{
	auto playerController = world->GetFirstPlayerController();
		 
	if (playerController)
	{
		AActor* actualTarget = playerController->GetViewTarget();
		TArray<AActor*> cameras;
		UGameplayStatics::GetAllActorsWithTag(this->GetWorld(), "StartCamera", cameras);		
		if (cameras.Num() > 0)
		{
			AActor* nextTarget = cameras[0];
			if (nextTarget != actualTarget)
			{
				FViewTargetTransitionParams params;
				params.BlendTime = 0;
				playerController->SetViewTarget(cameras[0], params);
			}
		}
	}
}
