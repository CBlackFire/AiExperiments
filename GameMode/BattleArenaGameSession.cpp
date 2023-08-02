// <copyright file="BattleArenaGameSession.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "GameMode/GameInstance/SoulShardsGameInstance.h"
#include "GameMode/BattleArenaGameMode.h"
#include "BattleArenaGameSession.h"

void ABattleArenaGameSession::RegisterServer()
{
	if (this->IsDedicatedServerLobby())
	{
		IOnlineSubsystem* const onlineSub = IOnlineSubsystem::Get();
		if (onlineSub)
		{
			IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
			if (sessions.IsValid())
			{
				if (this->GetNetMode() == ENetMode::NM_DedicatedServer)
				{
					USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(
						this->GetWorld()->GetGameInstance());
					gameInstance->GetServerGameInstanceManager()->HostSession(EGameType::EGTAuto, FGuid::NewGuid());
				}
			}
		}
	}
}

FString ABattleArenaGameSession::ApproveLogin(const FString& options)
{
	UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameSession::ApproveLogin() started:  Options: %s"), *options);

	if (this->IsDedicatedServerLobby())
	{
		UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameSession::ApproveLogin() executing in dedicated server mode"));

		FGameOptionsData gameOptions = UBaseGameInstanceManager::GetGameOptionsFromString(options);

		FString gameType = UGameplayStatics::ParseOption(options, TEXT("GameType"));
		if (gameOptions.GameType == EGameType::EGTAuto)
		{
			UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameSession::ApproveLogin() no game type specified"));
			return TEXT("No game type specified for dedicated server");
		}

		FString userName = UGameplayStatics::ParseOption(options, TEXT("Name"));

		USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(
			this->GetWorld()->GetGameInstance());
		return gameInstance->GetServerGameInstanceManager()->DedicatedServerApproveLogin(userName, gameOptions);
	}

	return FString();
}

void ABattleArenaGameSession::PostLogin(APlayerController* newPlayer)
{
	UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameSession::PostLogin() started"));

	if (this->IsDedicatedServerLobby())
	{

		USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(
			this->GetWorld()->GetGameInstance());
		return gameInstance->GetServerGameInstanceManager()->DedicatedServerPostLogin(newPlayer);
	}
}

bool ABattleArenaGameSession::IsDedicatedServerLobby()
{
	return this->GetNetMode() == ENetMode::NM_DedicatedServer;
}


void ABattleArenaGameSession::NotifyLogout(const APlayerController* controller)
{
	if (this->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(
			this->GetWorld()->GetGameInstance());
		gameInstance->GetServerGameInstanceManager()->DedicatedServerLogout(controller);
	}
}

