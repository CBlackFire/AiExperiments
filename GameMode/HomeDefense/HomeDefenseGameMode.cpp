// <copyright file="HomeDefenseGameMode.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "Player/Controller/BattleArenaPlayerController.h"
#include "GameMode/HomeDefense/HomeDefenseMainPhase.h"
#include "GameMode/HomeDefense/HomeDefensePreparePhase.h"
#include "Character/SoulCaster/Actor/SoulShardsCharacter.h"
#include "HomeDefenseGameMode.h"
#include <SoulShards\GameMode\GameInstance\BaseGameInstanceManager.h>

#include "SoulShardsHelper.hpp"

AHomeDefenseGameMode::AHomeDefenseGameMode()
{
}

void AHomeDefenseGameMode::InitPhases(FGameOptionsData gameOptions)
{
	UHomeDefensePreparePhase* preparePhase = NewObject<UHomeDefensePreparePhase>(this, UHomeDefensePreparePhase::StaticClass(), TEXT("HomeDefensePreparePhase"));
	UHomeDefenseMainPhase* mainPhase = NewObject<UHomeDefenseMainPhase>(this, UHomeDefenseMainPhase::StaticClass(), TEXT("HomeDefenseMainPhase"));

	this->AddPhase(preparePhase);
	this->AddPhase(mainPhase);

	float handicapMultiplier = 1.0f;

	if (gameOptions.Difficulty == EGameDifficulty::EGDEasy)
	{
		handicapMultiplier = 1.5f;
	}
	else if (gameOptions.Difficulty == EGameDifficulty::EGDMedium)
	{
		handicapMultiplier = 1.15f;
	}

	mainPhase->SetHandicapMultiplier(handicapMultiplier);
}

void AHomeDefenseGameMode::AddPlayerDefinitionsForAI(FPlayerData& vsPlayer, APlayerController* mainPlayerController)
{
	ABattleArenaPlayerController* defaultController = this->GetWorld()->SpawnActor<ABattleArenaPlayerController>(
		ABattleArenaPlayerController::StaticClass());
	defaultController->MarkAsAi();
	this->RegisterAiPlayerController(defaultController);

	FPlayerData newPlayerDefinition;
	newPlayerDefinition.PlayerDefinitionRoleType = EPlayerDefinitionRoleType::PDTAiChallenger;

	newPlayerDefinition.GameOptions.DeckId = vsPlayer.GameOptions.ContentId;

	if (newPlayerDefinition.GameOptions.DeckId.Len() == 0)
	{
		if (vsPlayer.GameOptions.GameType == EGameType::EGTHomeDefense || vsPlayer.GameOptions.GameType == EGameType::EGTOfflineHomeDefense)
		{
			ABattleArenaPlayerController* playerController = static_cast<ABattleArenaPlayerController*>(
				vsPlayer.PlayerControllerActor);
			newPlayerDefinition.GameOptions.DeckId = playerController->GetPlayerId();
		}
		else if (vsPlayer.GameOptions.GameType == EGameType::EGTOfflineChallengeHomeDefense)
		{
			// We don't need a deck id in this case
		}
		else
		{
			checkf(false, TEXT("DeckId is required when not playing self home defense"));
		}
	}

	newPlayerDefinition.SoulCasterName = newPlayerDefinition.GameOptions.DeckId;
	newPlayerDefinition.UserName = newPlayerDefinition.GameOptions.DeckId;

	TArray<FPlayerData>& playerDefinitions = this->GetPlayerDefinitions();

	//TODO:[Allegiance] This way of setting the allegiance will work while there is no co-op.
	FAllegianceData newAllegiance(playerDefinitions.Num(), playerDefinitions.Num());
	defaultController->SetAllegiance(newAllegiance);

	newPlayerDefinition.PlayerControllerActor = defaultController;

	newPlayerDefinition.Allegiance = newAllegiance;
	playerDefinitions.Add(newPlayerDefinition);

	ABattleArenaPlayerController* mainHttpPlayerController = static_cast<ABattleArenaPlayerController*>(mainPlayerController);

	switch (vsPlayer.GameOptions.GameType)
	{
		case EGameType::EGTHomeDefense:
		case EGameType::EGTOfflineHomeDefense:
		{
			FGetDeckEvent onGetDeckCompleted;
			onGetDeckCompleted.BindDynamic(defaultController, &ABattleArenaPlayerController::OnGetDeckCompleted);

			FHttpFailureEvent failureEvent;
			failureEvent.BindDynamic(defaultController, &AHttpEnabledPlayerController::OnGameServerHttpFailure);

			mainHttpPlayerController->GetHttpCallManager()->HttpGetHomeDefenseDeck(newPlayerDefinition.GameOptions.DeckId, onGetDeckCompleted, failureEvent);
			break;
		}

		case EGameType::EGTOfflineChallengeHomeDefense:
		case EGameType::EGTOfflineQuestHomeDefense:
		{
			if (newPlayerDefinition.GameOptions.DeckId.Len() == 0)
			{
				FGetUserContentIdEvent getContentIdCallback;
				getContentIdCallback.BindDynamic(this, &AHomeDefenseGameMode::OnGetDeckIdCompleted);

				FHttpFailureEvent failureEvent;
				failureEvent.BindDynamic(defaultController, &AHttpEnabledPlayerController::OnGameServerHttpFailure);

				mainHttpPlayerController->GetHttpCallManager()->HttpGetUserContentId(vsPlayer.GameOptions.GameType, EGameDifficulty::EGDAny, getContentIdCallback, failureEvent);
			}
			else
			{
				this->OnGetDeckIdCompleted(newPlayerDefinition.GameOptions.DeckId, vsPlayer.GameOptions.GameType);
			}
			break;
		}

		default:
		{
			checkf(false, TEXT("Unrecognized home defense type"));
		}
	}

	Super::AddPlayerDefinitionsForAI(vsPlayer, mainPlayerController);
}

void AHomeDefenseGameMode::OnGetDeckIdCompleted(FString contentId, EGameType gameType)
{
	ABattleArenaPlayerController* defaultController = static_cast<ABattleArenaPlayerController*>(this->GetDefaultPlayerController());
	FGetDeckEvent onGetDeckCompleted;
	onGetDeckCompleted.BindDynamic(defaultController, &ABattleArenaPlayerController::OnGetDeckCompleted);

	FHttpFailureEvent failureEvent;
	failureEvent.BindDynamic(defaultController, &AHttpEnabledPlayerController::OnGameServerHttpFailure);

	bool success;
	const FGameOptionsData gameOptions = this->GetGameOptions(success);
	check(success);

	ABattleArenaPlayerController* mainPlayerController =
		Cast<ABattleArenaPlayerController>(SoulShardsHelper::GetFirstLocalNonReplayPlayerController(this->GetWorld()));
	check(mainPlayerController);
	
	if (gameOptions.GameType == EGameType::EGTHomeDefense || gameOptions.GameType == EGameType::EGTOfflineHomeDefense)
	{
		mainPlayerController->GetHttpCallManager()->HttpGetHomeDefenseDeck(contentId, onGetDeckCompleted, failureEvent);
	}
	else
	{
		this->UpdatePlayerDeckId(contentId, 1);
		mainPlayerController->GetHttpCallManager()->HttpGetContentDeck(gameOptions.GameType, contentId, onGetDeckCompleted, failureEvent);
	}
}

void AHomeDefenseGameMode::OnGetDeckCompleted(APlayerController* playerController, FDeckData deck)
{
	ABattleArenaPlayerController* typedPlayerController = static_cast<ABattleArenaPlayerController*>(playerController);
	int playerIndex = typedPlayerController->GetAllegiance().PlayerIndex;

	// Gets the data for the game creator
	FPlayerData gameCreatorPlayerData = this->GetPlayerDefinitions()[0];

	// If this is the AI deck
	if (playerIndex == 1)
	{
		if ((gameCreatorPlayerData.GameOptions.Difficulty != EGameDifficulty::EGDHard) &&
			(gameCreatorPlayerData.GameOptions.GameType != EGameType::EGTOfflineHomeDefense))
		{
			FString handicapCardKey;

			if (gameCreatorPlayerData.GameOptions.Difficulty == EGameDifficulty::EGDEasy)
			{
				handicapCardKey = "SPL_WorldEnchant_DifficultyHandicap_Easy";
			}
			else if (gameCreatorPlayerData.GameOptions.Difficulty == EGameDifficulty::EGDMedium)
			{
				handicapCardKey = "SPL_WorldEnchant_DifficultyHandicap_Medium";
			}

			if (handicapCardKey.Len() > 0)
			{
				//Add nightmare enchants
				FCardData handicapCard;
				handicapCard.GameKey = handicapCardKey;
				deck.WorldEnchants.Add(handicapCard);
			}
		}

		// Make sure only central defensive soulwell is visible
		if (deck.SoulWellsData.UseSoulWellConfiguration)
		{
			check(deck.SoulWellsData.DefensiveSoulWellsData.Num() == 3);
			deck.SoulWellsData.DefensiveSoulWellsData[0].RemovedFromGame = false;
			deck.SoulWellsData.DefensiveSoulWellsData[1].RemovedFromGame = true;
			deck.SoulWellsData.DefensiveSoulWellsData[2].RemovedFromGame = true;
		}
	}

	Super::OnGetDeckCompleted(playerController, deck);

	// Add the level to the deck name
	if (playerIndex == 1)
	{
		this->GetPlayerDefinitions()[playerIndex].UserName += TEXT(" Home Defense");
	}
}

EGameResult AHomeDefenseGameMode::GetGameResult(
	int& player0HitPoints, 
	int& player1HitPoints,
	EAlternativeWinCondition& player0WinCondition,
	EAlternativeWinCondition& player1WinCondition)
{
	TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());

	TSmartListGroupIteratorTemplate<TDuelPlayer, false> itPlayer(battleArena->players);
	TDuelPlayer* player0 = itPlayer.At(0);
	TDuelPlayer* player1 = itPlayer.At(1);

	ASoulShardsCharacter* player0Caster = player0->GetSoulCaster();
	ASoulShardsCharacter* player1Caster = player1->GetSoulCaster();

	bool player0Died = player0Caster == nullptr;
	bool player1Died = player1Caster == nullptr;

	player0HitPoints = -1;
	player1HitPoints = -1;

	player0WinCondition = EAlternativeWinCondition::AWCNoWin;
	player1WinCondition = EAlternativeWinCondition::AWCNoWin;

	if (player0Caster && player0Caster->GetD20Character())
	{
		player0HitPoints = player0Caster->GetD20Character()->GetStats()->GetHitPoints()->GetValue()->ToInt();
		player0Died = player0HitPoints <= 0;
		player0WinCondition = player0Caster->GetAlternativeWinState();
	}

	if (player1Caster && player1Caster->GetD20Character())
	{
		player1HitPoints = player1Caster->GetD20Character()->GetStats()->GetHitPoints()->GetValue()->ToInt();
		player1Died = player1HitPoints <= 0;
		player1WinCondition = player1Caster->GetAlternativeWinState();
	}

	// if both players are dead, then it's a draw
	if (player0Died && player1Died)
	{
		return EGameResult::EGRDraw;
	}

	// if home defense survives, it's a victory for home defense
	if (!player1Died)
	{
		return EGameResult::EGRPlayer1Win;
	}

	// if home defense didn't survive, it's a victory for the player
	return EGameResult::EGRPlayer0Win;
}

bool AHomeDefenseGameMode::ShouldRecordReplay()
{
	return true;
}