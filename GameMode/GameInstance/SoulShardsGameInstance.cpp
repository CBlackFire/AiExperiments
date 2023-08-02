// <copyright file="SoulShardsGameInstance.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "Character/SoulCaster/Actor/SoulShardActor.h"
#include "GameMode/BattleArenaGameSession.h"
#include "Character/Component/CharacterParticleSystemComponent.h"
#include "Gui/Widget/UserConfirmationDialog.h"
#include "GameMode/BattleArenaGameMode.h"
#include "SoulShardsHelper.hpp"

#if UE_SERVER
#include "ThirdParty/PlayfabGSDK/include/gsdk.h"
#endif

#include "GameMode/GameInstance/SoulShardsGameInstance.h"



bool USoulShardsGameInstance::MoveFile(FString filePath, FString newPath)
{

#if WITH_EDITOR

	FPlatformFileManager::Get().GetPlatformFile().BypassSecurity(true);

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*filePath))
	{
		UE_LOG(SoulShardsLog, Warning, TEXT("Could Not Find File: (%s)."), *filePath);
		return false;
	}

	if (!FPlatformFileManager::Get().GetPlatformFile().MoveFile(*newPath, *filePath))
	{
		UE_LOG(SoulShardsLog, Warning, TEXT("Could not move file: (%s)."), *filePath);
		return false;
	}

#endif

	return true;

}

void USoulShardsGameInstance::CopyFile(FString filePath, FString newPath)
{

#if WITH_EDITOR

	FPlatformFileManager::Get().GetPlatformFile().BypassSecurity(true);

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*filePath))
	{
		UE_LOG(SoulShardsLog, Warning, TEXT("Could Not Find File: (%s)."), *filePath);
		return;
	}

	if (!FPlatformFileManager::Get().GetPlatformFile().CopyFile(*newPath, *filePath))
	{
		UE_LOG(SoulShardsLog, Warning, TEXT("Could not copy file: (%s)."), *filePath);
	}

#endif

}


USoulShardsGameInstance::USoulShardsGameInstance()
{
	this->ClientGameInstanceManager = CreateDefaultSubobject<UClientGameInstanceManager>(TEXT("ClientGameInstanceManager"));
	this->ClientGameInstanceManager->SetParentGameInstance(this);
	this->ReplayManager = CreateDefaultSubobject<UReplayManager>(TEXT("ReplayManager"));
	this->ReplayManager->SetParentGameInstance(this);
	this->TournamentManager = CreateDefaultSubobject<UTournamentManager>(TEXT("TournamentManager"));
	this->TournamentManager->SetParentGameInstance(this);
	this->ServerGameInstanceManager = CreateDefaultSubobject<UServerGameInstanceManager>(TEXT("ServerGameInstanceManager"));
	this->ServerGameInstanceManager->SetParentGameInstance(this);

	this->commandLine = FCommandLine::Get();

#if UE_SERVER

	if (this->ServerGameInstanceManager->GetUsePlayfabGsdk())
	{
		// Some operations should only be initialized when the game instance is the real deal, not CDO or Sekl
		if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
		{
			// Notigy Playfab GSDK that our server has started
			Microsoft::Azure::Gaming::GSDK::start();
		}
	}
#endif

	// Initialize Auto Settings
	//this->settingsManager = nullptr;
	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		// We store AutoSetting singleton here so it's not disposed.
		//this->settingsManager = USettingsManager::Get(true);
		//this->settingsManager = USettingsManager::Get();
		//USettingsManager::Get();
		//this->settingsManager->Init();

		/*this->cvarChangeListenerManager = UCVarChangeListenerManager::Get(true);
		this->globalKeyIconTagManager = UGlobalKeyIconTagManager::Get(true);
		this->inputMappingManager = UInputMappingManager::Get(true);*/

#if WITH_EDITOR
#else
#if UE_SERVER
#else
		if (!FParse::Value(FCommandLine::Get(), TEXT("-xsolla-login-token"), this->commandLine))
		{		
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Please login by using XSolla launcher")));
			GEngine->Exec(this->GetWorld(), TEXT("QUIT"));			
		}
#endif
#endif
	}
	
	this->DynamicSpellAssetContainer = CreateDefaultSubobject<UDynamicAssetContainer>(TEXT("DynamicSpellAssetContainer"));
	this->DynamicCasterAssetContainer = CreateDefaultSubobject<UDynamicAssetContainer>(TEXT("DynamicCasterAssetContainer"));
	this->DynamicMaterialAssetContainer = CreateDefaultSubobject<UDynamicAssetContainer>(TEXT("DynamicMaterialAssetContainer"));
	this->ScriptedLevelAssetContainer = CreateDefaultSubobject<UDynamicAssetContainer>(TEXT("ScriptedLevelAssetContainer"));
	this->MiscAssetContainer = CreateDefaultSubobject<UDynamicAssetContainer>(TEXT("MiscAssetContainer"));
}

UClass* USoulShardsGameInstance::GetClassByString(FString classPath, bool checkResult)
{
	check(IsInGameThread());
	UClass* result = StaticLoadClass(UObject::StaticClass(), nullptr, *classPath, nullptr, LOAD_None, nullptr);
	UE_LOG(SoulShardsLog, Log, TEXT("USoulShardsGameInstance::GetClassByString(): Class being allocated: %s"), *classPath);
	check(result || !checkResult);	
	return result;
}

void USoulShardsGameInstance::LeaveGame(bool submitMatchResults)
{
	UWorld* world = this->GetWorld();
	if (this->ServerGameInstanceManager)
	{
		this->ServerGameInstanceManager->LeaveGame(submitMatchResults);
	} 
	else if (world->GetNetMode() != ENetMode::NM_DedicatedServer)
	{	
		this->ClientGameInstanceManager->LeaveGame(submitMatchResults);
	}
}

void USoulShardsGameInstance::LeaveGameAndSubmitMatchResults()
{
	this->LeaveGame(true /* submitMatchResults */);
}

UDynamicAssetContainer* USoulShardsGameInstance::GetDynamicSpellAssetContainer()
{
	if (this->DynamicSpellAssetContainer->GetAssetMap().Num() == 0)
	{		
		this->DarkVoidSpellManifestClass = this->GetClassByString("/Game/Dev/Gameplay/Manifests/U_SpellManifest_DarkVoid.U_SpellManifest_DarkVoid_C");
		this->IceVoidSpellManifestClass = this->GetClassByString("/Game/Dev/Gameplay/Manifests/U_SpellManifest_IceVoid.U_SpellManifest_IceVoid_C");
		this->NoRealmSpellManifestClass = this->GetClassByString("/Game/Dev/Gameplay/Manifests/U_SpellManifest_NoRealm.U_SpellManifest_NoRealm_C");
		this->WorldEnchantSpellManifestClass = this->GetClassByString("/Game/Dev/Gameplay/Manifests/U_SpellManifest_WorldEnchant.U_SpellManifest_WorldEnchant_C");

		this->DynamicSpellAssetContainer->AddFrom(
			Cast<UDynamicAssetContainer>(NewObject< UDynamicAssetContainer>(this, this->DarkVoidSpellManifestClass)));
		this->DynamicSpellAssetContainer->AddFrom(
			Cast<UDynamicAssetContainer>(NewObject< UDynamicAssetContainer>(this, this->IceVoidSpellManifestClass)));
		this->DynamicSpellAssetContainer->AddFrom(
			Cast<UDynamicAssetContainer>(NewObject< UDynamicAssetContainer>(this, this->NoRealmSpellManifestClass)));
		this->DynamicSpellAssetContainer->AddFrom(
			Cast<UDynamicAssetContainer>(NewObject< UDynamicAssetContainer>(this, this->WorldEnchantSpellManifestClass)));
			
	}
	
	return this->DynamicSpellAssetContainer;
}

UDynamicAssetContainer* USoulShardsGameInstance::GetDynamicCasterAssetContainer()
{
	if (this->DynamicCasterAssetContainer->GetAssetMap().Num() == 0)
	{
		this->CasterManifestClass = this->GetClassByString("/Game/Dev/Gameplay/Manifests/U_CasterManifest.U_CasterManifest_C");

		this->DynamicCasterAssetContainer->AddFrom(
			Cast<UDynamicAssetContainer>(NewObject< UDynamicAssetContainer>(this, this->CasterManifestClass)));
	}

	return this->DynamicCasterAssetContainer;
}

UDynamicAssetContainer* USoulShardsGameInstance::GetDynamicMaterialAssetContainer()
{
	if (this->DynamicMaterialAssetContainer->GetAssetMap().Num() == 0)
	{
		this->MaterialManifestClass = this->GetClassByString("/Game/Dev/Gameplay/Manifests/U_MaterialManifest.U_MaterialManifest_C");
		this->DynamicMaterialAssetContainer->AddFrom(
			Cast<UDynamicAssetContainer>(NewObject< UDynamicAssetContainer>(this, this->MaterialManifestClass)));
	}
	return this->DynamicMaterialAssetContainer;
}

UDynamicAssetContainer* USoulShardsGameInstance::GetScriptedLevelAssetContainer()
{
	if (this->ScriptedLevelAssetContainer->GetAssetMap().Num() == 0)
	{
		this->ScriptedLevelManifestClass = this->GetClassByString("/Game/Dev/Gameplay/Manifests/U_ScriptedLevelManifest.U_ScriptedLevelManifest_C");

		this->ScriptedLevelAssetContainer->AddFrom(
			Cast<UDynamicAssetContainer>(NewObject< UDynamicAssetContainer>(this, this->ScriptedLevelManifestClass)));
	}

	return this->ScriptedLevelAssetContainer;
}

UDynamicAssetContainer* USoulShardsGameInstance::GetMiscAssetContainer()
{
	if (this->MiscAssetContainer->GetAssetMap().Num() == 0)
	{
		this->MiscManifestClass = this->GetClassByString("/Game/Dev/Gameplay/Manifests/U_MiscAssetManifest.U_MiscAssetManifest_C");

		this->MiscAssetContainer->AddFrom(
			Cast<UDynamicAssetContainer>(NewObject< UDynamicAssetContainer>(this, this->MiscManifestClass)));
	}

	return this->MiscAssetContainer;
}

void USoulShardsGameInstance::Shutdown()
{
	this->LeaveGame();
}

UClientGameInstanceManager* USoulShardsGameInstance::GetClientGameInstanceManager()
{
	return this->ClientGameInstanceManager;
}

UReplayManager* USoulShardsGameInstance::GetReplayManager()
{
	return this->ReplayManager;
}

UTournamentManager* USoulShardsGameInstance::GetTournamentManager()
{
	return this->TournamentManager;
}

UServerGameInstanceManager* USoulShardsGameInstance::GetServerGameInstanceManager()
{
	return this->ServerGameInstanceManager;
}

UBaseGameInstanceManager* USoulShardsGameInstance::GetCurrentGameInstanceManager()
{
	return this->CurrentGameInstanceManager;
}

FString USoulShardsGameInstance::GetCusmtomUserId()
{
	return this->CustomUserId;
}

void USoulShardsGameInstance::SetCusmtomUserId(FString customUserId)
{
	this->CustomUserId = customUserId;
}

void USoulShardsGameInstance::SetCurrentGameInstanceManager(UBaseGameInstanceManager* currentGameInstanceManager)
{
	this->CurrentGameInstanceManager = currentGameInstanceManager;
}

bool USoulShardsGameInstance::GetIsDedicatedServer()
{
	UWorld* world = this->GetWorld();
	if (world)
	{
		return world->GetNetMode() == ENetMode::NM_DedicatedServer;
	}

	return false;
}

bool USoulShardsGameInstance::GetIsQueuedForMultiplayer()
{
	return this->ClientGameInstanceManager->GetIsQueuedForMultiPlayer();
}

FString USoulShardsGameInstance::PopCommandLine()
{
	FString toReturn = this->commandLine;
	this->commandLine = TEXT("");
	return toReturn;
}

UMapContainer* USoulShardsGameInstance::GetMapContiner()
{
	
	if (this->MapContainer == nullptr)
	{
		this->MapContainerClass = this->GetClassByString("/Game/Dev/Gameplay/Manifests/U_MapContainer.U_MapContainer_C");
		this->MapContainer = NewObject<UMapContainer>(this, this->MapContainerClass);
	}
	return this->MapContainer;
}

UDeckContainer* USoulShardsGameInstance::GetDeckContiner()
{
	if (this->DeckContainer == nullptr)
	{
		this->DeckContainerClass = this->GetClassByString("/Game/Dev/Gameplay/Manifests/U_DeckContainer.U_DeckContainer_C");
		this->DeckContainer = NewObject<UDeckContainer>(this, this->DeckContainerClass);
	}

	return this->DeckContainer;
}

UCameraScriptContainer* USoulShardsGameInstance::GetCameraScriptContainer()
{
	if (this->CameraScriptContainer == nullptr)
	{
		this->CameraScriptContainerClass = this->GetClassByString("/Game/Dev/Gameplay/Manifests/U_CameraScriptContainer.U_CameraScriptContainer_C");
		this->CameraScriptContainer = NewObject<UCameraScriptContainer>(this, this->CameraScriptContainerClass);
	}

	return this->CameraScriptContainer;
}

FMapData USoulShardsGameInstance::GetMapForGameType(EGameType gameType, FStringAssetReference specificMap, EGameRealm gameRealm)
{
	if (!specificMap.IsNull())
	{
		FMapData mapData;
		mapData.MapRealm = gameRealm;
		mapData.IsNightmareMap = UBaseGameInstanceManager::GetIsNightmareGameType(gameType);
		mapData.MapAsset = specificMap;
		mapData.ValidBattleArena = true;
		mapData.ValidMainMenu = false;
		return mapData;
	}

	if (UBaseGameInstanceManager::GetIsNightmareGameType(gameType))
	{
		return this->GetMapContiner()->GetNightmareMap(gameRealm);
	}
	else
	{
		return this->GetMapContiner()->GetBattleArenaMap(gameRealm);
	}

	return FMapData();
}

void USoulShardsGameInstance::FinishDestroy()
{
	/*if (this->settingsManager)
	{
		UCVarChangeListenerManager::ReleaseSingleton();
		UGlobalKeyIconTagManager::ReleaseSingleton();
		UInputMappingManager::ReleaseSingleton();
		USettingsManager::ReleaseSingleton();
	}*/

	//this->settingsManager = nullptr;

	// Finish destroy must go at the end by UE requirement
	Super::FinishDestroy();
}

