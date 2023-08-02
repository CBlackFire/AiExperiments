// <copyright file="BaseGameInstanceManager.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>


#include "SoulShards.h"
#include "GameMode/GameInstance/SoulShardsGameInstance.h"
#include "GameMode/SoulShardsGameMode.h"
#include "SoulShardsBaseHelper.hpp"
#include "BaseGameInstanceManager.h"


UBaseGameInstanceManager::UBaseGameInstanceManager()
{
	this->parentGameInstance = nullptr;
}


UWorld* UBaseGameInstanceManager::GetWorld() const
{
	if (this->parentGameInstance)
	{
		return this->parentGameInstance->GetWorld();
	}

	return nullptr;
}


void UBaseGameInstanceManager::SetCurrentGameInstanceManager()
{
	USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(parentGameInstance);
	gameInstance->SetCurrentGameInstanceManager(this);
}

FString UBaseGameInstanceManager::GetMapName(EGameType gameType, FGuid uniqueId, bool inProgress)
{
	FString mapName;

	if (UBaseGameInstanceManager::GetGameTypeUsesUniqueId(gameType))
	{
		TArray<uint8> guidByteArray;
		uint8* guidAsByteArray = (uint8*)(&uniqueId.A);
		for (int i = 0; i < 8; i++)
		{
			guidByteArray.Add(guidAsByteArray[i]);
		}

		guidAsByteArray = (uint8*)(&uniqueId.B);
		for (int i = 0; i < 8; i++)
		{
			guidByteArray.Add(guidAsByteArray[i]);
		}

		guidAsByteArray = (uint8*)(&uniqueId.C);
		for (int i = 0; i < 8; i++)
		{
			guidByteArray.Add(guidAsByteArray[i]);
		}

		guidAsByteArray = (uint8*)(&uniqueId.D);
		for (int i = 0; i < 8; i++)
		{
			guidByteArray.Add(guidAsByteArray[i]);
		}

		mapName = FBase64::Encode(guidByteArray).Left(22);
		mapName = mapName.Replace(TEXT("+"), TEXT("-"));
		mapName = mapName.Replace(TEXT("/"), TEXT("_"));

		FString testId = uniqueId.ToString();
		testId = testId.Append(TEXT("|"));
	}
	else
	{
		mapName = FString::FromInt((int)gameType);
	}

	mapName += TEXT("v");
	mapName += inProgress ? TEXT("1") : TEXT("0");
	return mapName;
}

EGameType UBaseGameInstanceManager::GetMatchingQueuedGameType(EGameType gameType)
{
	if (gameType == EGameType::EGTDuelGame)
	{
		return EGameType::EGTQueuedDuelGame;
	}
	else if (gameType == EGameType::EGTRankedDuelGame)
	{
		return EGameType::EGTQueuedRankedDuelGame;
	}

	return gameType;
}

EGameType UBaseGameInstanceManager::GetMatchingNonQueuedGameType(EGameType gameType)
{
	if (gameType == EGameType::EGTQueuedDuelGame)
	{
		return EGameType::EGTDuelGame;
	}
	else if (gameType == EGameType::EGTQueuedRankedDuelGame)
	{
		return EGameType::EGTRankedDuelGame;
	}

	return gameType;
}

bool UBaseGameInstanceManager::GetIsQueuedGameType(EGameType gameType)
{
	return gameType == EGameType::EGTQueuedDuelGame || gameType == EGameType::EGTQueuedRankedDuelGame;
}

FString UBaseGameInstanceManager::GetGameModeForGameType(EGameType gameType)
{
	FString gameModePath = TEXT("/Script/SoulShards.");

	FString gameModeClass;

	switch (gameType)
	{
		case EGameType::EGTAuto:
		case EGameType::EGTDuelGame:
		case EGameType::EGTRankedDuelGame:
		case EGameType::EGTQueuedDuelGame:
		case EGameType::EGTQueuedRankedDuelGame:
		case EGameType::EGTFriendChallengeGame:
		{
			gameModeClass = TEXT("DuelGameMode");
			break;
		}

		case EGameType::EGTHomeDefense:
		case EGameType::EGTOfflineHomeDefense:
		case EGameType::EGTOfflineChallengeHomeDefense:
		case EGameType::EGTOfflineQuestHomeDefense:
		{
			gameModeClass = TEXT("HomeDefenseGameMode");
			break;
		}

		case EGameType::EGTOfflineChallengeNightmare:
		case EGameType::EGTOfflineNightmare:
		{
			gameModeClass = TEXT("NightmareGameMode");
			break;
		}

		case EGameType::EGTOfflineDeckBuilderTest:
		case EGameType::EGTOfflineScripted:
		{
			gameModeClass = TEXT("ScriptedGameMode");
			break;
		}

		case EGameType::EGTOfflineHomeBuilderTest:
		{
			gameModeClass = TEXT("HomeBuilderGameMode");
			break;				
		}
	}

	check(gameModeClass.Len() > 0);
	return gameModePath + gameModeClass;
}

FString UBaseGameInstanceManager::GetGameOptionsForGameType(EGameType gameType)
{
	FString gameOptions = TEXT("?game=") + UBaseGameInstanceManager::GetGameModeForGameType(gameType);
	return gameOptions;
}

FString UBaseGameInstanceManager::GetLobbyOptionsForGameType(FGameOptionsData gameOptionsData)
{
	FString gameTypeString = FString::FromInt((int)gameOptionsData.GameType);
	FString options = TEXT("?GameType=") + gameTypeString;

	if (UBaseGameInstanceManager::GetGameTypeUsesUniqueId(gameOptionsData.GameType))
	{
		options += TEXT("?GameId=") + gameOptionsData.GameId.ToString();
	}

	if (UBaseGameInstanceManager::GetGameTypeUsesContentId(gameOptionsData.GameType))
	{
		options += TEXT("?ContentId=") + gameOptionsData.ContentId;
	}

	if (UBaseGameInstanceManager::GetGameTypeUsesLevel(gameOptionsData.GameType))
	{
		options += TEXT("?Level=") + FString::FromInt(gameOptionsData.Level);
	}

	if (UBaseGameInstanceManager::GetGameTypeUsesRealm(gameOptionsData.GameType))
	{
		options += TEXT("?Realm=") + FString::FromInt((int)gameOptionsData.Realm);
	}

	options += TEXT("?DeckId=") + gameOptionsData.DeckId;

	options += TEXT("?ClientPlayerId=") + gameOptionsData.ClientPlayerId;

	FString activeBadgeString = FString::FromInt((int)gameOptionsData.ActiveBadge);
	options += TEXT("?ActiveBadge=") + activeBadgeString;
	return options;
}

FGameOptionsData UBaseGameInstanceManager::GetGameOptionsFromString(FString options)
{
	FGameOptionsData gameOptions;

	FString gameType = UGameplayStatics::ParseOption(options, TEXT("GameType"));
	if (gameType.Len() == 0)
	{
		gameOptions.GameType = EGameType::EGTAuto;
	}
	else
	{
		gameOptions.GameType = (EGameType)FCString::Atoi(*gameType);
		gameOptions.DeckId = UGameplayStatics::ParseOption(options, TEXT("DeckId"));

		FString gameGuid = UGameplayStatics::ParseOption(options, TEXT("GameId"));
		FGuid::Parse(gameGuid, gameOptions.GameId);

		FString level = UGameplayStatics::ParseOption(options, TEXT("Level"));
		if (level.Len() > 0)
		{
			gameOptions.Level = FCString::Atoi(*level);
		}

		FString realm = UGameplayStatics::ParseOption(options, TEXT("Realm"));
		if (realm.Len() > 0)
		{
			gameOptions.Realm = (EGameRealm)FCString::Atoi(*realm);
		}

		gameOptions.ClientPlayerId = UGameplayStatics::ParseOption(options, TEXT("ClientPlayerId"));

		FString activeBadge = UGameplayStatics::ParseOption(options, TEXT("ActiveBadge"));
		gameOptions.ActiveBadge = (EUserBadgeType)FCString::Atoi(*activeBadge);
	}

	return gameOptions;
}


bool UBaseGameInstanceManager::GetIsOnlineGameType(EGameType gameType)
{
	if ((gameType == EGameType::EGTOfflineHomeDefense) ||
		(gameType == EGameType::EGTOfflineChallengeBoss) ||
		(gameType == EGameType::EGTOfflineQuestHomeDefense) ||
		(gameType == EGameType::EGTOfflineChallengeHomeDefense) ||
		(gameType == EGameType::EGTOfflineChallengeNightmare) ||
		(gameType == EGameType::EGTOfflineChallengePuzzle) ||
		(gameType == EGameType::EGTOfflineNightmare) ||
		(gameType == EGameType::EGTOfflineDeckBuilderTest) ||
		(gameType == EGameType::EGTOfflineHomeBuilderTest) ||
		(gameType == EGameType::EGTOfflineScripted))
	{
		return false;
	}

	return true;
}


bool UBaseGameInstanceManager::GetSupportsScoreWidget(EGameType gameType)
{
	if ((gameType == EGameType::EGTOfflineChallengeNightmare) ||
		(gameType == EGameType::EGTOfflineNightmare) ||
		(gameType == EGameType::EGTOfflineQuestHomeDefense) ||
		(gameType == EGameType::EGTHomeDefense) ||
		(gameType == EGameType::EGTOfflineChallengeHomeDefense) ||		
		(gameType == EGameType::EGTOfflineHomeDefense))
	{
		return true;
	}

	return false;
}

FString UBaseGameInstanceManager::GetQueueNameForGameType(EGameType gameType)
{
	switch (gameType)
	{
		case EGameType::EGTRankedDuelGame:	
			return this->RanekdMultiplayerQueueName;
		case EGameType::EGTDuelGame:
			return this->MultiplayerQueueName;
	}

	return FString();
}

bool UBaseGameInstanceManager::GetIsNightmareGameType(EGameType gameType)
{
	if ((gameType == EGameType::EGTOfflineChallengeNightmare) ||
		(gameType == EGameType::EGTOfflineNightmare))
	{
		return true;
	}

	return false;
}

bool UBaseGameInstanceManager::GetIsHomeDefenseType(EGameType gameType)
{
	if ((gameType == EGameType::EGTOfflineQuestHomeDefense) ||
		(gameType == EGameType::EGTHomeDefense) ||
		(gameType == EGameType::EGTOfflineChallengeHomeDefense) ||
		(gameType == EGameType::EGTOfflineHomeDefense))
	{
		return true;
	}

	return false;
}

bool UBaseGameInstanceManager::GetIsMultiplayerGameType(EGameType gameType)
{
	if (gameType == EGameType::EGTDuelGame ||
		gameType == EGameType::EGTRankedDuelGame ||
		gameType == EGameType::EGTQueuedDuelGame ||
		gameType == EGameType::EGTQueuedRankedDuelGame ||
		gameType == EGameType::EGTFriendChallengeGame)
	{
		return true;
	}

	return false;
}

bool UBaseGameInstanceManager::GetGameTypeUsesUniqueId(EGameType gameType)
{
	if (gameType == EGameType::EGTFriendChallengeGame ||
		gameType == EGameType::EGTQueuedDuelGame ||
		gameType == EGameType::EGTQueuedRankedDuelGame)
	{
		return true;
	}

	return false;
}

bool UBaseGameInstanceManager::GetGameTypeUsesContentId(EGameType gameType)
{
	if (gameType == EGameType::EGTOfflineChallengeBoss ||
		gameType == EGameType::EGTOfflineQuestHomeDefense ||
		gameType == EGameType::EGTOfflineChallengeHomeDefense ||
		gameType == EGameType::EGTOfflineChallengeNightmare ||
		gameType == EGameType::EGTOfflineNightmare ||
		gameType == EGameType::EGTOfflineChallengePuzzle ||
		gameType == EGameType::EGTOfflineScripted)
	{
		return true;
	}

	return false;
}

bool UBaseGameInstanceManager::GetGameTypeUsesLevel(EGameType gameType)
{
	if (gameType == EGameType::EGTOfflineNightmare ||
		gameType == EGameType::EGTOfflineChallengeNightmare ||
		gameType == EGameType::EGTOfflineScripted)
	{
		return true;
	}

	return false;
}

bool UBaseGameInstanceManager::GetGameTypeUsesRealm(EGameType gameType)
{
	if (gameType == EGameType::EGTOfflineNightmare ||
		gameType == EGameType::EGTOfflineChallengeNightmare ||
		gameType == EGameType::EGTOfflineScripted)
	{
		return true;
	}

	return false;
}

FName UBaseGameInstanceManager::GetCurrentSessionName()
{
	return this->currentSessionName;
}

void UBaseGameInstanceManager::SetCurrentSessionName(FName sessionName)
{
	this->currentSessionName = sessionName;
}

TSharedPtr<class FOnlineSessionSettings> UBaseGameInstanceManager::GetSessionSettings()
{
	return this->sessionSettings;
}

void UBaseGameInstanceManager::SetSessionSettings(TSharedPtr<class FOnlineSessionSettings> settings)
{
	this->sessionSettings = settings;
}

void UBaseGameInstanceManager::LeaveGame(bool submitMatchResults)
{
	if (this->GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		ASoulShardsGameMode* gameMode = Cast<ASoulShardsGameMode>(this->GetWorld()->GetAuthGameMode());
		if (gameMode && gameMode->IsMatchInProgress())
		{
			gameMode->EndMatch();
		}

		GEngine->Exec(this->GetWorld(), TEXT("QUIT"));
	}
	else
	{
		// If we are not on dedicated server, go back to main menu
		AHttpEnabledPlayerController* playerController = static_cast<AHttpEnabledPlayerController*>(parentGameInstance->GetFirstLocalPlayerController());
		if (SoulShardsBaseHelper::IsValidActor(playerController))
		{
			playerController->NavigateToMainMenuMap(submitMatchResults);
		}
	}
}

UGameInstance* UBaseGameInstanceManager::GetParentGameInstance()
{
	return this->parentGameInstance;
}

void UBaseGameInstanceManager::SetParentGameInstance(UGameInstance* gameInstance)
{
	this->parentGameInstance = gameInstance;
	
	// Initializes tick timer
	FTimerHandle UnusedHandle;
	// Initializes tick timer
	gameInstance->GetTimerManager().SetTimer(
		UnusedHandle,
		this,
		&UBaseGameInstanceManager::Tick,
		1,
		true);
}

void UBaseGameInstanceManager::Tick()
{

}