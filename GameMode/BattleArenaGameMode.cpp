// <copyright file="BattleArenaGameMode.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "Player/Controller/BattleArenaPlayerController.h"
#include "Player/Controller/ReplayPlayerController.h"
#include "Character/SoulCaster/Actor/SoulShardsCharacter.h"
#include "GameMode/GameInstance/SoulShardsGameInstance.h"
#include "Player/Controller/MainMenuPlayerController.h"
#include "AiEngine/AiEngine/TAIPlayer.hpp"
#include "AiEngine/AiEngine/TAIEngine.hpp"
#include "SoulShardsHelper.hpp"
#include "BattleArenaGameMode.h"

ABattleArenaGameMode::ABattleArenaGameMode()
{	
	UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameMode::ABattleArenaGameMode() started"));
	this->DefaultPawnClass = APawn::StaticClass();

	this->CoolDownManager = CreateDefaultSubobject<UCoolDownManagerComponent>(TEXT("CoolDownManagerComponent"));

	this->PlayerControllerClass = ABattleArenaPlayerController::StaticClass();
	this->ReplaySpectatorPlayerControllerClass = AReplayPlayerController::StaticClass();

	this->matchStartTime = -1.0f;

	this->aiContentPhaseIndex = 0;

	this->disabledGameLevelFeatures.Add(EConfigurableGameLevelFeature::CGFSwitchTeamPerspective);
}

int ABattleArenaGameMode::GetAiContentPhaseIndex()
{
	return this->aiContentPhaseIndex;
}

void ABattleArenaGameMode::SetAiContentPhaseIndex(int index)
{
	this->aiContentPhaseIndex = index;
}

void ABattleArenaGameMode::AddPhase(UBattleArenaGamePhase* phase)
{
	phase->SetPhaseIndex(this->Phases.Num());
	this->Phases.Add(phase);
}

APlayerController* ABattleArenaGameMode::GetDefaultPlayerController()
{
	if (this->AiPlayerControllers.Num() > 0)
	{
		return this->AiPlayerControllers[0];
	}

	return nullptr;
}

TSubclassOf<APawn> ABattleArenaGameMode::GetSoulCasterPawnClass(FAllegianceData allegiance)
{
	

	// if there is a player definition for the allegiance
	if (allegiance.PlayerIndex < this->PlayerDefinitions.Num())
	{
		FPlayerData& playerDefinition = this->PlayerDefinitions[allegiance.PlayerIndex];
		TSubclassOf<APawn> toReturn = playerDefinition.Deck.Caster;
		if (toReturn != nullptr)
		{
			return toReturn;
		}
	}

	if (this->SoulCasterPawnClass == nullptr)
	{
		this->SoulCasterPawnClass = SoulShardsHelper::GetClassByString("/Game/Helgast/GamePlay/Characters/SoulCaster/BAS_GuruNim.BAS_GuruNim_C", this);
	}
	
	return this->SoulCasterPawnClass;	
}

int ABattleArenaGameMode::GetMinPlayersToStartMatch()
{
	return 1;
}

bool ABattleArenaGameMode::ReadyToStartMatch_Implementation()
{
	const int readyPlayerCount = this->GetReadyPlayerCount();
	if (readyPlayerCount >= this->GetMinPlayersToStartMatch() && readyPlayerCount == this->PlayerDefinitions.Num())
	{
		const UWorld* world = this->GetWorld();
		this->matchStartTime = world->GetTimeSeconds();		
		UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameMode::ReadyToStartMatch_Implementation(): Ready to start match."));
		return true;
	}

	//UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameMode::ReadyToStartMatch_Implementation(): Not ready to start match."));
	return false;
}


void ABattleArenaGameMode::OnMatchStarted_Delayed()
{
	const ENetMode netMode = this->GetNetMode();
	
	if (this->GetBattleArena() == nullptr)
	{
		const UWorld* world = this->GetWorld();
		this->matchStartTime = world->GetTimeSeconds();
		// Start replay recording only if we're not on dedicated server.
		if (netMode != ENetMode::NM_DedicatedServer)
		{
			USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(this->GetGameInstance());
			ABattleArenaLevelScriptActor* levelScriptActor = Cast<ABattleArenaLevelScriptActor>(world->GetLevelScriptActor());
			bool shouldRecordReplay = levelScriptActor && levelScriptActor->GetLevelData()->ShouldRecordReplay;
			if (shouldRecordReplay)
			{
				shouldRecordReplay = this->ShouldRecordReplay();
			}
				
			if (shouldRecordReplay)
			{
				gameInstance->GetReplayManager()->StartRecordingGameReplay();
			}
		}

		const FPlayerData playerData = this->PlayerDefinitions[0];
		this->InitPhases(playerData.GameOptions);
		this->InitBattleArena();						
	}				
}

/// <summary>
/// Copies the player data from game instance.
/// </summary>
/// <param name="userName">Name of the user.</param>
void ABattleArenaGameMode::CopyPlayerDataFromGameInstance(AController* playerController)
{
	FString userName = playerController->PlayerState->GetPlayerName();

	FPlayerData tmpPlayerData;
	// We can only copy the data if the user already has a name
	if (userName.Len() > 0)
	{	
		tmpPlayerData.UserName = userName;
		tmpPlayerData.FriendlyUserName = SoulShardsBaseHelper::GetFriendlyTitleDisplayName(userName);
		USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(this->GetWorld()->GetGameInstance());

		TMap<FString, FPlayerData> loggedInPlayers = gameInstance->GetServerGameInstanceManager()->GetLoggedInPlayers();
		FPlayerData* loggedInPlayer = loggedInPlayers.Find(tmpPlayerData.UserName);
		if (loggedInPlayer != nullptr)
		{
			ABattleArenaPlayerController* battleArenaPlayerController = static_cast<ABattleArenaPlayerController*>(playerController);
			const FAllegianceData gameModePlayerAllegiance = battleArenaPlayerController->GetAllegiance();
			const int gameModePlayerIndex = gameModePlayerAllegiance.PlayerIndex;

			// If the player already exists we update the data
			if ((gameModePlayerIndex >= 0) && (this->PlayerDefinitions.Num() > gameModePlayerIndex))
			{
				this->PlayerDefinitions[gameModePlayerIndex] = *loggedInPlayer;
				this->PlayerDefinitions[gameModePlayerIndex].Allegiance = gameModePlayerAllegiance;
			}
			else
			{				
				// Otherwise we add the player definition
				tmpPlayerData.GameOptions = loggedInPlayer->GameOptions;
				//TODO:[Allegiance] Will not work for co-op
				tmpPlayerData.Allegiance = FAllegianceData(this->PlayerDefinitions.Num(), this->PlayerDefinitions.Num());
				this->PlayerDefinitions.Add(tmpPlayerData);
			}
		}
	}
	else
	{
		//TODO:[Allegiance] Will not work for co-op
		// If there is no user information we simply add an emtpy player data
		tmpPlayerData.Allegiance = FAllegianceData(this->PlayerDefinitions.Num(), this->PlayerDefinitions.Num());
		this->PlayerDefinitions.Add(tmpPlayerData);
	}
}

AActor* ABattleArenaGameMode::ChoosePlayerStart_Implementation(AController* player)
{
	ABattleArenaPlayerController* typedPlayerController = Cast<ABattleArenaPlayerController>(player);
	if (typedPlayerController)
	{	
		FAllegianceData allegiance = typedPlayerController->GetAllegiance();
		bool isAi = false;

		// If the allegiance for the player have not been defined, we assume it will just be inserted by CopyPlayerDataFromGameInstance
		if (!allegiance.IsValid())
		{
			//TODO:[Allegiance] Will not work for co-op.  Why is this even here?
			typedPlayerController->SetAllegiance(FAllegianceData(this->PlayerDefinitions.Num(), this->PlayerDefinitions.Num()));
			allegiance = typedPlayerController->GetAllegiance();
		}

		if (allegiance.PlayerIndex < PlayerDefinitions.Num())
		{
			FPlayerData playerDefinition = this->PlayerDefinitions[allegiance.PlayerIndex];
			isAi = playerDefinition.IsAiPlayer();
		}

		// Ais have their player data already defined and don't need copying data from the game instance
		if (!isAi)
		{
			this->CopyPlayerDataFromGameInstance(player);
		}

		UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameMode::ChoosePlayerStart() for player %i."), typedPlayerController->GetAllegiance().PlayerIndex);

		FString playerStartName("PlayerStart");
		playerStartName.AppendInt(allegiance.PlayerIndex);
		return SoulShardsBaseHelper::GetActorByName<APlayerStart>(playerStartName, this->GetWorld());
	}

	return nullptr;
}

void ABattleArenaGameMode::PostLogin(APlayerController * newPlayer)
{
	Super::PostLogin(newPlayer);

	// On player joined is called via a different route when in dedicated server.
	if (this->GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		this->OnPlayerJoined(newPlayer);
	}
}

/*void ABattleArenaGameMode::HandleSeamlessTravelPlayer(AController*& controller) 
{
	Super::HandleSeamlessTravelPlayer(controller);

	
	this->OnPlayerJoined(controller);
}*/

void ABattleArenaGameMode::OnPlayerJoined(AController* newPlayer)
{
	check(newPlayer->GetClass()->IsChildOf(ABattleArenaPlayerController::StaticClass()));
	ABattleArenaPlayerController* typedPlayerController = static_cast<ABattleArenaPlayerController*>(newPlayer);

	/* This shouldn't be necessary */
	APawn* initialPawn = typedPlayerController->GetPawn(); 
	if (initialPawn)
	{
		typedPlayerController->UnPossess();
		initialPawn->Destroy();
	}

	if (!typedPlayerController->GetAllegiance().IsValid())
	{		
		//TODO:[Allegiance] Will not work for co-op
		typedPlayerController->SetAllegiance(FAllegianceData(this->PlayerDefinitions.Num(), this->PlayerDefinitions.Num()));		
	}

	this->CopyPlayerDataFromGameInstance(newPlayer);


	this->PlayerDefinitions[typedPlayerController->GetAllegiance().PlayerIndex].PlayerControllerActor = typedPlayerController;

	typedPlayerController->ClientPostLogin();

	// Only after the last player has been added, AIS are added
	if (this->PlayerDefinitions.Num() == this->GetMinPlayersToStartMatch())
	{
		this->AddPlayerDefinitionsForAI(this->PlayerDefinitions[typedPlayerController->GetAllegiance().PlayerIndex], typedPlayerController);
	}

	UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameMode::OnPlayerJoined(): Executed OnPlayerJoined player %i."), typedPlayerController->GetAllegiance().PlayerIndex);
}

void ABattleArenaGameMode::ReSpawnPlayer(APlayerController* controller, bool gameStartFlow)
{
	check(controller->GetClass()->IsChildOf(ABattleArenaPlayerController::StaticClass()));
	ABattleArenaPlayerController* typedPlayerController = static_cast<ABattleArenaPlayerController*>(controller);
	const FAllegianceData allegiance = typedPlayerController->GetAllegiance();
	UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameMode::ReSpawnPlayer(): ReSpawning player %i."), allegiance.PlayerIndex);
	TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());

	ASoulShardsCharacter* oldCaster = Cast<ASoulShardsCharacter>(typedPlayerController->GetPawn());
	
	// If the game is starting we don't need to replace the old caster.
	if (gameStartFlow)
	{
		FPlayerData playerData = this->PlayerDefinitions[typedPlayerController->GetAllegiance().PlayerIndex];

		//First look for default decks
		USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(this->GetWorld()->GetGameInstance());
		FDeckData* deckContainer = gameInstance->GetDeckContiner()->GetDeckById(playerData.GameOptions.DeckId);
		if (deckContainer)
		{
			FDeckData playerDeck = *deckContainer;
			playerDeck.OwnerNetId = playerData.GameOptions.ClientPlayerId;
			playerDeck.PlayerName = playerData.UserName;
			typedPlayerController->OnGetDeckCompleted(playerDeck);		
		} else
		{ 
			//If not a default deck looks it up from web site
			FGetDeckEvent onGetDeckCompleted;
			onGetDeckCompleted.BindDynamic(typedPlayerController, &ABattleArenaPlayerController::OnGetDeckCompleted);

			FHttpFailureEvent failureEvent;
			failureEvent.BindDynamic(typedPlayerController, &AHttpEnabledPlayerController::OnGameServerHttpFailure);

			typedPlayerController->GetHttpCallManager()->HttpGetDeck(playerData.GameOptions.DeckId, playerData.GameOptions.ClientPlayerId, onGetDeckCompleted, failureEvent);
		}
	} else
	{
		check(oldCaster);
		TDuelPlayer* duelPlayer = battleArena->GetPlayerForSoulCaster(*oldCaster);

        // Destroy the old totem so the new one can spawn
        oldCaster->DestroyTotem();		
		oldCaster->Destroy(true);

		FPlayerData& playerDefinition = duelPlayer->GetPlayerDefinition();
		battleArena->InitializePlayer(playerDefinition);
		
	}
	
	ABattleArenaLevelScriptActor* leveScriptActor = Cast<ABattleArenaLevelScriptActor>(this->GetWorld()->GetLevelScriptActor());
	leveScriptActor->OnPlayerRespawned();
}

int ABattleArenaGameMode::GetReadyPlayerCount()
{
	int count = 0;
	for (int i = 0; i < this->PlayerDefinitions.Num(); i++)
	{
		if (this->PlayerDefinitions[i].ReadyToStartMatch)
		{
			count++;
		}
	}

	return count;
}

void ABattleArenaGameMode::InitBattleArena()
{
	UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameMode::InitBattleArena() Start."));

	UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameMode::InitBattleArena(): Creating battle arena."));
	TServerBattleArena& battleArena = *new TServerBattleArena(
		this,
		D20RoundDuration,
		1,
		FVector(0.0f, 0.0f, 0.0f));

	this->SetBattleArena(&battleArena);

	TAiEngine& aiEngine = *new TAiEngine(battleArena);
	battleArena.aiEngine = &aiEngine;

	UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameMode::InitBattleArena(): Initializing battle arena."));
	battleArena.InitializePlayers();

	UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameMode::InitBattleArena(): Calling battle arena start game."));
	battleArena.StartGame();

	UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameMode::InitBattleArena() End."));
}

void ABattleArenaGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ABattleArenaGameMode::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	for (auto& controller : this->AiPlayerControllers)
	{
		controller->Destroy();
	}

	this->AiPlayerControllers.Empty();

	Super::EndPlay(endPlayReason);
	
	this->PlayerDefinitions.Reset();
}


bool ABattleArenaGameMode::ShouldSpawnAtStartSpot(AController* player)
{
	return ASoulShardsGameMode::ShouldSpawnAtStartSpot(player);
}

void ABattleArenaGameMode::Tick(float deltaTime)
{
	// While legacy code executes, we will keep GC frome executing
	FGCScopeGuard scopeGuard;
	for (int i = 0; i < this->SpawnQueue.Num(); i++)
	{
		AActor* toSpawn = this->SpawnQueue[i].Actor;
		if (SoulShardsBaseHelper::IsValidActor(toSpawn))
		{
			UGameplayStatics::FinishSpawningActor(toSpawn, FTransform::Identity);
		}
	}

	this->SpawnQueue.Reset();	
	
	TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());

	if (battleArena != nullptr)
	{
		UBattleArenaGamePhase* currentGamePhase = battleArena->GetCurrentGamePhase();

		// Trigger queued phase events (one per frame per object)
		if (currentGamePhase && this->PhaseEventQueue.Num() > 0)
		{
			TArray<FPhaseEventData> toRemove;
			TSet<UBattleArenaGamePhase*> triggered;			
			
			for (int i = 0; i < this->PhaseEventQueue.Num(); i++)
			{
				const FPhaseEventData currentEvent = this->PhaseEventQueue[i];

				if (currentEvent.GamePhase == nullptr ||				
					currentEvent.GamePhase != currentGamePhase ||
					!currentEvent.GamePhase->IsValidLowLevel())					
				{
					toRemove.Add(this->PhaseEventQueue[i]);
					continue;
				}
				
				if (currentEvent.Target != nullptr && !currentEvent.Target->IsValidLowLevel())
				{
					toRemove.Add(currentEvent);
					continue;
				}

				if (!triggered.Find(currentEvent.GamePhase))
				{
					currentGamePhase->TriggerPhaseEvent(currentEvent.EventName, currentEvent.Target);
					toRemove.Add(this->PhaseEventQueue[i]);
					triggered.Add(currentGamePhase);
				}
			}

			// Cleanup
			for (int i = 0; i < toRemove.Num(); i++)
			{
				FPhaseEventData current = toRemove[i]; 
				this->PhaseEventQueue.Remove(current);
			}
		}
				
		battleArena->Execute();
	} else
	{
		if (this->matchStartTime >= 0.0f)
		{
			const UWorld* world = this->GetWorld();
			float currentTime = world->GetTimeSeconds();

			// Wait two seconds before starting game.
			if (currentTime >= this->matchStartTime + 2.0f)
			{
				this->OnMatchStarted_Delayed();	
			}		
		}
	}

	Super::Tick(deltaTime);
}

TArray<UBattleArenaGamePhase*>& ABattleArenaGameMode::GetPhases()
{
	return this->Phases;
}


bool ABattleArenaGameMode::CheckGameEndCondition()
{
	for (int i = 0; i < this->Phases.Num(); i++)
	{
		UBattleArenaGamePhase& actualPhase = *this->Phases[i];
		if (actualPhase.CheckGameEndCondition())
		{
			return true;
		}
	}

	return false;
}

void ABattleArenaGameMode::InitPhases(FGameOptionsData gameOptions)
{

}

UBattleArenaGamePhase& ABattleArenaGameMode::GetFirstPhase()
{
	check(this->Phases.Num() > 0);

	int currentPhaseIndex = 0;
	UBattleArenaGamePhase* currentPhase = this->Phases[currentPhaseIndex];
	bool enabled = this->Phases[currentPhaseIndex]->GetIsEnabled();
	while (!enabled && currentPhase)
	{
		currentPhaseIndex++;
		currentPhase = this->Phases[currentPhaseIndex];
		if (currentPhase)
		{
			enabled = currentPhase->GetIsEnabled();
		}
	}

	check(currentPhase != nullptr);

	return *currentPhase;
}

UBattleArenaGamePhase* ABattleArenaGameMode::GetNextPhase(UBattleArenaGamePhase& currentPhase)
{
	int currentPhaseIndex = this->Phases.Find(&currentPhase);
	check(currentPhaseIndex >= 0);
	currentPhaseIndex++;

	if (currentPhaseIndex < this->Phases.Num())
	{
		UBattleArenaGamePhase* currentPhase = this->Phases[currentPhaseIndex];
		bool enabled = this->Phases[currentPhaseIndex]->GetIsEnabled();
		while (!enabled && currentPhase)
		{
			currentPhaseIndex++;
			currentPhase = this->Phases[currentPhaseIndex];
			if (currentPhase)
			{
				enabled = currentPhase->GetIsEnabled();
			}
		}

		return currentPhase;
	} 
	else
	{
		// This will trigger the game to end
		return nullptr;
	}
}

void ABattleArenaGameMode::RegisterAiPlayerController(APlayerController* value)
{
	this->AiPlayerControllers.Add(value);
}

void ABattleArenaGameMode::AddPlayerDefinitionsForAI(FPlayerData& vsPlayer, APlayerController* mainPlayerController)
{
	//Adds the neutral player which will be required for every match
	ABattleArenaPlayerController* defaultController = this->GetWorld()->SpawnActor<ABattleArenaPlayerController>(
		ABattleArenaPlayerController::StaticClass());
	defaultController->MarkAsAi();
	this->RegisterAiPlayerController(defaultController);

	FPlayerData newPlayerDefinition;
	newPlayerDefinition.PlayerDefinitionRoleType = EPlayerDefinitionRoleType::PDTAiNeutral;

	newPlayerDefinition.SoulCasterName = FString("Neutral");
	newPlayerDefinition.UserName = FString("Neutral");

	TArray<FPlayerData>& playerDefinitions = this->GetPlayerDefinitions();

	//TODO:[Allegiance] This way of setting the allegiance will work while there is no co-op.
	FAllegianceData newAllegiance(playerDefinitions.Num(), playerDefinitions.Num());

	defaultController->SetAllegiance(newAllegiance);
	newPlayerDefinition.PlayerControllerActor = defaultController;

	newPlayerDefinition.Allegiance = newAllegiance;
	newPlayerDefinition.ReadyToStartMatch = true;
	playerDefinitions.Add(newPlayerDefinition);
}

void ABattleArenaGameMode::AddToSpawnQueue(AActor* actor)
{
	FSpawnQueueData spawnQueueData;
	spawnQueueData.Actor = actor;
	spawnQueueData.Class = actor->GetClass();
	this->SpawnQueue.Add(spawnQueueData);
}

bool ABattleArenaGameMode::IsBattleArenaGameInProgress()
{
	TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (battleArena)
	{
		return battleArena->GetBattleArenaState() == TBattleArenaState::BASGameInProgress;
	}

	return false;
}

void ABattleArenaGameMode::EndCurrentPhase()
{
	TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (battleArena)
	{
		battleArena->EndCurrentPhase();
	}
}

void ABattleArenaGameMode::EndMatch()
{
	TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (battleArena)
	{
		battleArena->EndGame(false);
	}

	Super::EndMatch();
	
	if (this->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(this->GetWorld()->GetGameInstance());
		ABattleArenaGameMode* gameMode = this;
		FTimerHandle timerHandle;
		FTimerDelegate timerCallback;
		timerCallback.BindUFunction(
			gameInstance,
			GET_FUNCTION_NAME_CHECKED(USoulShardsGameInstance, LeaveGameAndSubmitMatchResults));
		
		this->GetWorld()->GetTimerManager().SetTimer(timerHandle,timerCallback,10.0f, false);
	}
}

TArray<FPlayerData>& ABattleArenaGameMode::GetPlayerDefinitions()
{
	return this->PlayerDefinitions;
}

void ABattleArenaGameMode::OnGetDeckCompleted(APlayerController* playerController, FDeckData deck)
{
	// Updates the deck for the player definition
	ABattleArenaPlayerController* typedPlayerController = static_cast<ABattleArenaPlayerController*>(playerController);
	FAllegianceData allegiance = typedPlayerController->GetAllegiance(); 
	int playerIndex = allegiance.PlayerIndex;
	check(playerIndex >= 0);

	bool success = false;
	FGameOptionsData gameOptions = this->GetGameOptions(success);
	check(success);

	typedPlayerController->SetActiveBadge(this->PlayerDefinitions[playerIndex].GameOptions.ActiveBadge);

	this->PlayerDefinitions[playerIndex].UserName = deck.PlayerName;
	this->PlayerDefinitions[playerIndex].Deck = deck;	
	this->PlayerDefinitions[playerIndex].ReadyToStartMatch = true;
}

EGameResult ABattleArenaGameMode::GetGameResult(
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

	if (player0Died && player1Died)
	{
		return EGameResult::EGRDraw;
	}
	else if (player0Died)
	{
		return EGameResult::EGRPlayer1Win;
	}
	else if (player1Died)
	{
		return EGameResult::EGRPlayer0Win;
	}
	else if (player0WinCondition != EAlternativeWinCondition::AWCNoWin && player1WinCondition != EAlternativeWinCondition::AWCNoWin)
	{
		return EGameResult::EGRDraw;
	}
	else if (player0WinCondition != EAlternativeWinCondition::AWCNoWin)
	{
		return EGameResult::EGRPlayer0Win;
	} 
	else if (player1WinCondition != EAlternativeWinCondition::AWCNoWin)
	{
		return EGameResult::EGRPlayer1Win;
	}

	// otherwise player 0 wins
	return EGameResult::EGRDraw;
}

void ABattleArenaGameMode::CreateAiRulesFromPlayerData(IBattleArena& battleArena, FPlayerData playerData, TListGroupTemplate<TPlayerAiRule>& toReturn)
{
	TServerBattleArena* typedBattleArena = static_cast<TServerBattleArena*>(&battleArena);

	//TODO: if a card is played in the first execute an exception is thrown, then we add this rule to avoid this issue.
	TPlayerAiRule& firstRule = *new TPlayerAiRule(*typedBattleArena);
	firstRule.SetManaTrigger(10);
	firstRule.SetDeckPhase(0);
	toReturn.AddLast(firstRule);

	for (auto& worldEnchantData : playerData.Deck.WorldEnchants)
	{
		// Adds a rule that allow to cast a spell after 1.5 seconds of the last cast.
		FDeckActionData actionData;
		actionData.ActionType = EDeckActionType::DATCard;
		actionData.CardData = worldEnchantData;
		actionData.CardData.SoulWellIndex = 0;

		TPlayerAiRule& rule = *new TPlayerAiRule(*typedBattleArena, &actionData, playerData.Allegiance.PlayerIndex);
		rule.SetDeckPhase(0);
		rule.SetLastCastTimeTrigger(1.5f);
		toReturn.AddLast(rule);

		int requiredMana = rule.GetMana();
		rule.SetManaTrigger(requiredMana);
	}

	// We iterate through all phases
	int phaseIndex = 0;

	for (auto& phaseData : playerData.Deck.Phases)
	{
		// And for each phase through all actions
		for (auto& actionData : phaseData.Actions)
		{
			// Only cards are allowed in preparation phase.
			if (((phaseIndex == 0) && (actionData.ActionType == EDeckActionType::DATCard)) || phaseIndex > 0)
			{
				// Adds a rule that allow to cast a spell after 1.5 seconds of the last cast.
				TPlayerAiRule& rule = *new TPlayerAiRule(*typedBattleArena, &actionData, playerData.Allegiance.PlayerIndex);
				toReturn.AddLast(rule);

				// Preparation phase cast all cards immediately.  Only normal phases will use mana triggers
				if (phaseIndex != 0)
				{
					int requiredMana = rule.GetMana();
					rule.SetManaTrigger(requiredMana);
				}
				else
				{
					rule.SetLastCastTimeTrigger(1.5f);
				}

				rule.SetDeckPhase(phaseIndex);
			}
		}

		phaseIndex++;
	}
}

IAIPlayer* ABattleArenaGameMode::CreateAiPlayer(FPlayerData& playerData)
{
	return new TAIPlayer(*this->GetBattleArena());
}

float ABattleArenaGameMode::GetMatchStartTime()
{
	return this->matchStartTime;
}

UCoolDownManagerComponent* ABattleArenaGameMode::GetCoolDownManager()
{
	return this->CoolDownManager;
}

FGameOptionsData ABattleArenaGameMode::GetGameOptions(bool& success)
{
	if (PlayerDefinitions.Num() > 0)
	{
		success = true;
		return PlayerDefinitions[0].GameOptions;
	}

	success = false;
	return FGameOptionsData();
}

void ABattleArenaGameMode::PostSeamlessTravel()
{
	UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameMode::PostSeamlessTravel()."));
	
	Super::PostSeamlessTravel();

	if (this->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaGameMode::PostSeamlessTravel(): Detected dedicated server."));
		auto gameInstance = static_cast<USoulShardsGameInstance*>(this->GetGameInstance());
		UServerGameInstanceManager* serverInstance = gameInstance->GetServerGameInstanceManager();
		serverInstance->OnServerTravelFinished();
	}
}

void ABattleArenaGameMode::TriggerPhaseEvent(FString eventName, UObject* target)
{
	TServerBattleArena* typedBattleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (typedBattleArena)
	{
		UBattleArenaGamePhase* currentGamePhase = typedBattleArena->GetCurrentGamePhase();
		if (currentGamePhase)
		{
			const FPhaseEventData eventData(currentGamePhase, target, eventName);
			this->PhaseEventQueue.Add(eventData);
		}
	}
}

bool ABattleArenaGameMode::GetIsMatchResultsSubmissionConfirmed()
{
	USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(this->GetWorld()->GetGameInstance());

	if (gameInstance && gameInstance->GetServerGameInstanceManager())
	{
		return gameInstance->GetServerGameInstanceManager()->IsHttpRequestQueueEmpty();
	}

	return false;
}

FMatchResultData ABattleArenaGameMode::GetMatchResultData()
{
	TServerBattleArena* typedBattleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (typedBattleArena)
	{
		return typedBattleArena->GetMatchResultData();
	}

	return FMatchResultData();
}

void ABattleArenaGameMode::UpdatePlayerDeckId(FString deckId, int playerIndex)
{
	FPlayerData& playerDefinition = this->PlayerDefinitions[playerIndex];
	playerDefinition.GameOptions.DeckId = deckId;
}

void ABattleArenaGameMode::OnSubmitMatchResultsCompleted(bool success)
{
	TServerBattleArena* typedBattleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (typedBattleArena)
	{
		return typedBattleArena->OnSubmitMatchResultsCompleted(success);
	}

}

UClass* ABattleArenaGameMode::GetDefaultPawnClassForController_Implementation(AController* inController)
{
	ABattleArenaPlayerController* controller = Cast<ABattleArenaPlayerController>(inController);
	if (controller)
	{
		return this->GetSoulCasterPawnClass(controller->GetAllegiance());
	}

	return Super::GetDefaultPawnClassForController(inController);
}

void ABattleArenaGameMode::RestartPlayer(AController* newPlayer)
{
	/*ABattleArenaPlayerController* controller = Cast<ABattleArenaPlayerController>(newPlayer);
	bool callBase = controller == nullptr;
	if (!callBase)
	{
		callBase = !controller->GetAllegiance().IsNeutral();
	}

	if (callBase)
	{
		Super::RestartPlayer(newPlayer);
	}*/

	// We override this to avoid automatic pawn creation.  We want to create all pawns during game flow
	// to avoid multi-player sync issues. 
}

ABuildingLocator* ABattleArenaGameMode::GetBuildingLocator(ASoulWellActor* soulWellActor, bool totemBuildings)
{
	TServerBattleArena* serverBattleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (serverBattleArena)
	{
		TArray<ABuildingLocator*> buildingLocators = serverBattleArena->GetAllowedBuildingLocators(totemBuildings);

		for (auto& itBuildingLocator : buildingLocators)
		{
			if (itBuildingLocator->GetRelatedSoulWell() == soulWellActor)
			{
				return itBuildingLocator;
			}
		}
	}

	return nullptr;
}

void ABattleArenaGameMode::ResetBattleArena(TSubclassOf<class AFxCollection> creatureDespawnFx)
{
	TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	check(battleArena);

	TArray<ASpell*> spellExceptions;
	battleArena->ResetBattleArena(false, spellExceptions, creatureDespawnFx);
}

void ABattleArenaGameMode::GetSpellPipelineMask(int& value)
{
	value = 0;
	TServerBattleArena* typedBattleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (typedBattleArena)
	{
		value = typedBattleArena->GetSpellPipelineMask();
	}
}
void ABattleArenaGameMode::SetSpellPipelineMask(int value)
{
	TServerBattleArena* typedBattleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (typedBattleArena)
	{
		typedBattleArena->SetSpellPipelineMask(value);
	}
}

void ABattleArenaGameMode::SetGameLevelFeatureEnabled(EConfigurableGameLevelFeature gameLevelFeature, bool enabled)
{
	if (!enabled)
	{
		this->disabledGameLevelFeatures.Add(gameLevelFeature);
	}
	else
	{
		this->disabledGameLevelFeatures.Remove(gameLevelFeature);
	}
}

bool ABattleArenaGameMode::GetGameLevelFeatureEnabled(EConfigurableGameLevelFeature gameLevelFeature)
{
	return !disabledGameLevelFeatures.Contains(gameLevelFeature);
}

float ABattleArenaGameMode::GetMinDistanceToTotem()
{
	return this->MinDistanceToTotem;
}

void ABattleArenaGameMode::TriggerLevelEvent(FName eventName)
{
	ABattleArenaLevelScriptActor* leveScriptActor = Cast<ABattleArenaLevelScriptActor>(this->GetWorld()->GetLevelScriptActor());
	if (leveScriptActor)
	{
		leveScriptActor->OnLevelEventTriggered(eventName);
	}
}

void ABattleArenaGameMode::GetCreaturesInArea(FVector location, FRotator orientation, EAreaType areaType, float area,
    TArray<ABattleArenaCharacter*>& results)
{
	TServerBattleArena* typedBattleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (typedBattleArena)
	{
		TD20Location d20Location(location);
		FVector direction = orientation.Vector();
		typedBattleArena->GetCreaturesInArea(d20Location, direction, areaType, area, results);
	}
}

void ABattleArenaGameMode::GetEnemyCreaturesInArea(
	ABattleArenaCharacter* character,
	FVector location,
	FRotator orientation,
	EAreaType areaType,
	float area,
	bool excludeUnreachable,
	TArray<ABattleArenaCharacter*>& results)
{
	TServerBattleArena* typedBattleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (typedBattleArena)
	{
		TD20Location d20Location(location);
		FVector direction = orientation.Vector();
		results = typedBattleArena->GetEnemyCreaturesInArea(*character, d20Location, direction, areaType, area, excludeUnreachable);
	}	
}
void ABattleArenaGameMode::GetAllyCreaturesInArea(
	ABattleArenaCharacter* character,
	FVector location,
	FRotator orientation,
	EAreaType areaType,
	float area,
	bool excludeUnreachable,
	TArray<ABattleArenaCharacter*>& results)
{
	TServerBattleArena* typedBattleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (typedBattleArena)
	{
		TD20Location d20Location(location);
		FVector direction = orientation.Vector();
		results = typedBattleArena->GetAllyCreaturesInArea(*character, d20Location, direction, areaType, area, excludeUnreachable);
	}
}

void ABattleArenaGameMode::GetReachableEnemies(ABattleArenaCharacter* character,TArray<ABattleArenaCharacter*>& results)
{
	TServerBattleArena* typedBattleArena = static_cast<TServerBattleArena*>(this->GetBattleArena());
	if (typedBattleArena)
	{
		results = typedBattleArena->GetReachableEnemies(character);
	}
}

ABattleArenaGameMode::~ABattleArenaGameMode()
{	
}
