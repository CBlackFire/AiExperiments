// <copyright file="ScriptedGameMode.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "Player/Controller/BattleArenaPlayerController.h"
#include "GameMode/DuelGameMainPhase.h"
#include "GameMode/PreparePhase.h"
#include "Character/SoulCaster/Actor/SoulShardsCharacter.h"
#include "GameMode/GameInstance/SoulShardsGameInstance.h"
#include "ScriptedGameMode.h"

AScriptedGameMode::AScriptedGameMode()
{
	this->PhaseContainer = nullptr;
}

UPhaseContainer* AScriptedGameMode::GetPhaseContainer(FGameOptionsData gameOptions)
{
	if (this->PhaseContainer == nullptr)
	{
		USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(this->GetWorld()->GetGameInstance());
		UDynamicAssetContainer* scriptedLevelContainer = gameInstance->GetScriptedLevelAssetContainer();
		UClass* phaseContainerClass = scriptedLevelContainer->GetAssetClass(FName(*gameOptions.ContentId));
		if (phaseContainerClass)
		{
			//UPhaseContainer* phaseContainer = CreateDefaultSubobject<UPhaseContainer>(TEXT("PhaseContainer"), phaseContainerClass);
			this->PhaseContainer = NewObject<UPhaseContainer>(this, phaseContainerClass, TEXT("PhaseContainer"));
		}
	}

	check(this->PhaseContainer);
	return this->PhaseContainer;
}

void AScriptedGameMode::InitPhases(FGameOptionsData gameOptions)
{
	UPhaseContainer* phaseContainer = this->GetPhaseContainer(gameOptions);
	if (phaseContainer)
	{
		TArray<UBattleArenaGamePhase*>& phases = phaseContainer->GetPhaseArray();
		TArray<UTriggeredGamePhase*>& triggerPhases = phaseContainer->GetTriggerPhaseArray();

		for (auto& phase : phases)
		{
			this->AddPhase(phase);
		}

		for (auto& phase : triggerPhases)
		{
			this->TriggerPhases.Add(phase);
		}

		this->SetAiContentPhaseIndex(phaseContainer->GetContenDeckStartPhaseIndex());
	}
	else
	{
		UPreparePhase* preparePhase = NewObject<UPreparePhase>(this, UPreparePhase::StaticClass(), TEXT("DuelGamePreparePhase"));
		UDuelGameMainPhase* mainPhase = NewObject<UDuelGameMainPhase>(this, UDuelGameMainPhase::StaticClass(), TEXT("DuelGameMainPhase"));

		this->AddPhase(preparePhase);
		this->AddPhase(mainPhase);
	}

	Super::InitPhases(gameOptions);
}

void AScriptedGameMode::AddPlayerDefinitionsForAI(FPlayerData& vsPlayer, APlayerController* mainPlayerController)
{
	ABattleArenaPlayerController* defaultController = this->GetWorld()->SpawnActor<ABattleArenaPlayerController>(
		ABattleArenaPlayerController::StaticClass());
	defaultController->MarkAsAi();
	this->RegisterAiPlayerController(defaultController);

	FPlayerData newPlayerDefinition;
	newPlayerDefinition.PlayerDefinitionRoleType = EPlayerDefinitionRoleType::PDTAiChallenger;

	UPhaseContainer* phaseContainer = this->GetPhaseContainer(vsPlayer.GameOptions);
	newPlayerDefinition.GameOptions.DeckId = vsPlayer.GameOptions.ContentId;

	if (vsPlayer.GameOptions.ScriptedGameType == EGameType::EGTHomeDefense || vsPlayer.GameOptions.ScriptedGameType == EGameType::EGTOfflineHomeDefense)
	{
		ABattleArenaPlayerController* playerController = static_cast<ABattleArenaPlayerController*>(
			vsPlayer.PlayerControllerActor);
		newPlayerDefinition.GameOptions.DeckId = playerController->GetPlayerId();
	}
	else
	{
		if (phaseContainer && phaseContainer->GetContentDeckId().Len() > 0)
		{
			newPlayerDefinition.GameOptions.DeckId = phaseContainer->GetContentDeckId();
		}
	}

	newPlayerDefinition.SoulCasterName = phaseContainer->GetPlayerName();
	newPlayerDefinition.UserName = phaseContainer->GetPlayerName();

	TArray<FPlayerData>& playerDefinitions = this->GetPlayerDefinitions();

	//TODO:[Allegiance] This way of setting the allegiance will work while there is no co-op.
	FAllegianceData newAllegiance(playerDefinitions.Num(), playerDefinitions.Num());

	defaultController->SetAllegiance(newAllegiance);
	newPlayerDefinition.PlayerControllerActor = defaultController;

	newPlayerDefinition.Allegiance = newAllegiance;
	playerDefinitions.Add(newPlayerDefinition);	

	USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(this->GetWorld()->GetGameInstance());
	FDeckData* deckContainer = gameInstance->GetDeckContiner()->GetDeckById(newPlayerDefinition.GameOptions.DeckId);
	if (deckContainer)
	{
		FDeckData playerDeck = *deckContainer;
		playerDeck.PlayerName = newPlayerDefinition.UserName;
		defaultController->OnGetDeckCompleted(playerDeck);
	}
	else
	{
		FGetDeckEvent onGetDeckCompleted;
		onGetDeckCompleted.BindDynamic(defaultController, &ABattleArenaPlayerController::OnGetDeckCompleted);

		FHttpFailureEvent failureEvent;
		failureEvent.BindDynamic(defaultController, &AHttpEnabledPlayerController::OnGameServerHttpFailure);

		ABattleArenaPlayerController* mainHttpPlayerController = static_cast<ABattleArenaPlayerController*>(mainPlayerController);
		mainHttpPlayerController->GetHttpCallManager()->HttpGetHomeDefenseDeck(newPlayerDefinition.GameOptions.DeckId, onGetDeckCompleted, failureEvent);
	}

	Super::AddPlayerDefinitionsForAI(vsPlayer, mainPlayerController);
}

EGameResult AScriptedGameMode::GetGameResult(
	int& player0HitPoints, 
	int& player1HitPoints,
	EAlternativeWinCondition& player0WinCondition,
	EAlternativeWinCondition& player1WinCondition)
{
	TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (battleArena)
	{
		bool success;
		FGameOptionsData gameOptions = this->GetGameOptions(success);
		check(success);
		if (gameOptions.ScriptedGameType == EGameType::EGTHomeDefense || gameOptions.ScriptedGameType == EGameType::EGTOfflineHomeDefense)
		{
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

		if (battleArena->GetCurrentGamePhase() == nullptr)
		{
			return EGameResult::EGRPlayer0Win;
		}
	}

	return EGameResult::EGRPlayer1Win;
}

void AScriptedGameMode::Tick(float deltaTime)
{
	TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (battleArena)
	{
		UScriptedGamePhase* scriptedPhase = Cast<UScriptedGamePhase>(battleArena->GetCurrentGamePhase());
		bool allowPhaseTrigger = scriptedPhase == nullptr || (scriptedPhase->GetAllowTriggerPhase());
		if (allowPhaseTrigger && this->currentTriggeredPhase == nullptr)
		{
			this->currentTriggeredPhase = this->CheckForTriggeringPhase();
			if (this->currentTriggeredPhase != nullptr)
			{
				this->currentTriggeredPhase->StartPhase();
			}
		}

		if (this->currentTriggeredPhase != nullptr)
		{
			if (!allowPhaseTrigger || this->currentTriggeredPhase->CheckPhaseEndCondition())
			{
				this->currentTriggeredPhase->EndPhase();
				this->currentTriggeredPhase = nullptr;
			}
		}
	}

	Super::Tick(deltaTime);
}

/// <summary>
/// Checks if a phase is waiting to be triggered.
/// </summary>
UTriggeredGamePhase* AScriptedGameMode::CheckForTriggeringPhase()
{
	bool triggered = false;
	int i = 0;
	for (int i = 0; i < this->TriggerPhases.Num() && !triggered; i++)
	{		
		UTriggeredGamePhase* phase = this->TriggerPhases[i];
		if (phase->ShouldTrigger())
		{
			return phase;
		}
	}

	return nullptr;
}

void AScriptedGameMode::OnGetDeckCompleted(APlayerController* playerController, FDeckData deck)
{
	ABattleArenaPlayerController* typedPlayerController = static_cast<ABattleArenaPlayerController*>(playerController);
	int playerIndex = typedPlayerController->GetAllegiance().PlayerIndex;

	Super::OnGetDeckCompleted(playerController, deck);

	// Sets deck name.  This needs to be done here because OnGetDeckCompleted in the base class sets the deck name
	if (playerIndex == 1)
	{
		TArray<FPlayerData>& playerDataArray = this->GetPlayerDefinitions();
		playerDataArray[playerIndex].UserName = playerDataArray[playerIndex].SoulCasterName;
	}
}

bool AScriptedGameMode::ShouldRecordReplay()
{
	UPhaseContainer* phaseContainer = this->PhaseContainer;
	if (phaseContainer && phaseContainer->GetForceReplayRecord())
	{
		return true;
	}

	return Super::ShouldRecordReplay();	
}

void AScriptedGameMode::TriggerPhaseEvent(FString eventName, UObject* target)
{
	Super::TriggerPhaseEvent(eventName, target);

	for (auto& phase : this->TriggerPhases)
	{
		phase->TriggerPhaseEvent(eventName, target);
	}	
}