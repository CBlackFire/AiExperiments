// <copyright file="NightmareGameMode.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "Character/SoulCaster/Actor/SoulShardsCharacter.h"
#include "Player/Controller/BattleArenaPlayerController.h"
#include "GameMode/DuelGameMainPhase.h"
#include "GameMode/PreparePhase.h"
#include "GameMode/Nightmare/TNightmareAiPlayer.hpp"
#include "GameMode/Nightmare/TNightmareChallengeAiPlayer.hpp"
#include "GameMode/GameInstance/SoulShardsGameInstance.h"
#include "SoulShardsHelper.hpp"
#include "NightmareGameMode.h"

ANightmareGameMode::ANightmareGameMode()
{
	this->Level = 0;
	this->SoulPointsDebt = 0;
	this->DefenseSoulPointsEarned = 0;
}

EGameResult ANightmareGameMode::GetGameResult(
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
		player0WinCondition = player1Caster->GetAlternativeWinState();
	}

	bool isSurvival = false;
	UPhaseContainer* phaseContainer = this->GetPhaseContainer(FGameOptionsData());
	if (phaseContainer)
	{
		isSurvival = phaseContainer->GetIsSurvival();
	}

	if (isSurvival)
	{
		// if player survives until the end, they win
		if (battleArena->GetCurrentGamePhase() == nullptr && !player0Died)
		{
			return EGameResult::EGRPlayer0Win;
		}
				
		return EGameResult::EGRPlayer1Win;
	}
	else
	{
		// if nightmare survives, it's a victory for nightmare
		if (!player1Died)
		{
			// If there are no alternative win conditions this is a win for nightamre
			if (player0WinCondition == EAlternativeWinCondition::AWCNoWin)
			{
				return EGameResult::EGRPlayer1Win;
			}
			else
			{
				// The player can still win by alternative condition.
				return EGameResult::EGRPlayer0Win;
			}
		}
		
		return EGameResult::EGRPlayer0Win;
	}
}

void ANightmareGameMode::OnGetDeckCompleted(APlayerController* playerController, FDeckData deck)
{
	ABattleArenaPlayerController* typedPlayerController = static_cast<ABattleArenaPlayerController*>(playerController);
	int playerIndex = typedPlayerController->GetAllegiance().PlayerIndex;

	// Gets the data for the game creator
	FPlayerData gameCreatorPlayerData = this->GetPlayerDefinitions()[0];

	// If this is the AI deck
	if (playerIndex == 0)
	{
		bool success = false;
		FGameOptionsData gameOptions = this->GetGameOptions(success);
		check(success);
		this->Level = gameOptions.Level;

		for (auto& nightmareProgressionWorldEnchantData : this->GetNightmareProgressionWorldEnchantContainer()->GetNightmareProgressionWorldEnchants(this->Level))
		{
			FCardData nightmareProgressionWorldEnchant;

			nightmareProgressionWorldEnchant.CanShuffle = false;
			nightmareProgressionWorldEnchant.SpellClass = nightmareProgressionWorldEnchantData.SpellClass;
			nightmareProgressionWorldEnchant.GameKey = nightmareProgressionWorldEnchantData.SpellClass.GetDefaultObject()->SpellId;
			deck.Cards.Insert(nightmareProgressionWorldEnchant, 0);
		}
	}

	Super::OnGetDeckCompleted(playerController, deck);

	// Add the level to the deck name
	if (playerIndex == 1)
	{
		this->GetPlayerDefinitions()[playerIndex].UserName += TEXT(" ") + FString::FromInt((this->Level / 5) + 1);
	}
}

bool ANightmareGameMode::ShouldRecordReplay()
{
	return true;
}

UNightmareProgressionWorldEnchantContainer* ANightmareGameMode::GetNightmareProgressionWorldEnchantContainer()
{
	if (this->NightmareProgressionWorldEnchantContainer == nullptr)
	{
		const TSubclassOf<UNightmareProgressionWorldEnchantContainer> containerClass = SoulShardsHelper::GetClassByString(
            "/Game/Dev/Gameplay/Manifests/U_NightmareProgressionWorldEnchantContainer.U_NightmareProgressionWorldEnchantContainer_C", this); 
		this->NightmareProgressionWorldEnchantContainer =
			NewObject<UNightmareProgressionWorldEnchantContainer>(this, *containerClass, TEXT("ProgressionContainer"));		
	}

	return this->NightmareProgressionWorldEnchantContainer;
}