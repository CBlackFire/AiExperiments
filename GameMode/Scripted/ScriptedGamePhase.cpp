// <copyright file="ScriptedGamePhase.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "GameMode/BattleArenaGameMode.h"
#include "Cosmetic/ClientTextNotificationEffect.h"
#include "Character/SoulCaster/Actor/SoulShardsCharacter.h"
#include "Character/SoulCaster/Actor/BattleDeckActor.h"
#include "AiEngine/AiEngine/TAIEngine.hpp"
#include "Player/Controller/PlayerInputController.h"
#include "Player/Controller/BattleArenaPlayerController.h"
#include "BattleEngine/D20/Effect/Gameplay/TD20DestroyEffect.hpp"
#include "SoulShardsHelper.hpp"
#include "ScriptedGamePhase.h"

UScriptedGamePhase::UScriptedGamePhase()
{
	this->PhaseDuration = -1;
	this->EnablePhaseNotification = false;
	this->AllowTriggerPhase = false;
}

int UScriptedGamePhase::GetPhaseDuration()
{
	return this->PhaseDuration;
}

ASoulWellActor* UScriptedGamePhase::GetEnabledSoulWell(FAllegianceData allegiance, ESoulWellType soulWellType, int soulWellIndex, bool bypassEnabled)
{
	TServerBattleArena* battleArena = this->GetBattleArena();
	if (battleArena)
	{
		ASoulWellActor* toReturn = battleArena->GetSoulWell(soulWellType, soulWellIndex);
		if (toReturn != nullptr)
		{
			if ((toReturn->GetIsEnabled(allegiance) || bypassEnabled) && !toReturn->GetIsRemovedFromGame())
			{
				return toReturn;
			}
		}
	}
	
	TArray<ASoulWellActor*> soulWells = this->GetSoulWells();
	TArray<ASoulWellActor*> filteredSoulWells;
		
	// filter soul wells
	for (auto& soulWell : soulWells)
	{
		if ((soulWell->GetIsEnabled(allegiance) || bypassEnabled) && !soulWell->GetIsRemovedFromGame())
		{
			filteredSoulWells.Add(soulWell);
		}
	}

	if (filteredSoulWells.Num() > 0)
	{
		// tries to find a soulwell in the same lane
		for (auto& soulWell : filteredSoulWells)
		{
			if (soulWell->GetSoulWellType() == soulWellType)
			{
				return soulWell;
			}
		}

		int randomSouWellIndex = rand() % filteredSoulWells.Num();
		{
			return filteredSoulWells[randomSouWellIndex];
		}
	}

	return nullptr;
}

TArray<ASoulWellActor*> UScriptedGamePhase::GetSoulWells()
{
	TArray<ASoulWellActor*> toReturn;
	TServerBattleArena* battleArena = this->GetBattleArena();
	if (battleArena)
	{
		TArray<ASoulWellActor*> soulWellResults;
		battleArena->GetSoulWells(soulWellResults);		

		for (auto& itSoulWells: soulWellResults)
		{
			toReturn.Add(itSoulWells);
		}
	}

	return toReturn;	
}

bool UScriptedGamePhase::GetAllowTriggerPhase()
{
	return this->AllowTriggerPhase;
}

void UScriptedGamePhase::DestroyCreature(ABattleArenaCharacter* creature)
{
	ABattleArenaGameMode* gameMode = Cast<ABattleArenaGameMode>(this->GetWorld()->GetAuthGameMode());
	ABattleArenaPlayerController* playerController = SoulShardsHelper::GetFirstLocalNonReplayPlayerController(this->GetWorld());
	if (gameMode && playerController)
	{
		TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(gameMode->GetBattleArena());
		ASoulShardsCharacter* caster = Cast<ASoulShardsCharacter>(playerController->GetPawn());
		
		if (battleArena && battleArena)
		{		
			TD20DestroyEffect destroyEffect(battleArena->GetTime(), *creature, ED20DamageEnergyType::D20DETSoul, caster);
			destroyEffect.ApplyEffect(ESpellApplyUndoCause::AUCEffectDirectApply);
		}
	}
}

void UScriptedGamePhase::ExecutePhaseData(FPhaseStartData phaseData)
{
	if (!phaseData.IgnorePhaseData)
	{
		// Sets view target camera
		this->SetViewTarget(phaseData.StartCameraTag);

		ABattleArenaGameMode* gameMode = Cast<ABattleArenaGameMode>(this->GetWorld()->GetAuthGameMode());
		if (gameMode)
		{
			TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(gameMode->GetBattleArena());
			if (battleArena)
			{	
				// Appply auto advance configuration
				if (phaseData.ApplyAutoAdvanceConfiguration)
				{
					for (int i = 0; i < phaseData.AutoAdvanceConfiguration.Num() && i < 3; i++)
					{
						battleArena->SetAutoAdvanceEnabled(i, phaseData.AutoAdvanceConfiguration[i]);
					}
				}

				// Sets AI player hand
				if (phaseData.PhaseData.Actions.Num() > 0)
				{
					//TODO:[Co-op] The game currently supports only a single challenger
					int aiChallengerIndex = SoulShardsHelper::GetAiChallengerIndex(battleArena->GetPlayerDefinitions());
					if (aiChallengerIndex >= 0)
					{
						TAiEngine* aiEngine = static_cast<TAiEngine*>(battleArena->aiEngine);
						TSmartArrayGroupIteratorTemplate<TAIPlayer, false> it(aiEngine->playerList);
						TAIPlayer* aiPlayer = it.At(aiChallengerIndex);
						if (aiPlayer)
						{
							aiPlayer->SetPlayerAiRulesFromPhase(phaseData.PhaseData, phaseData.RequiresManaToCast, 1);
						}
					}
				}

				// Set SoulWell configuration
				if (phaseData.SoulWellsData.UseSoulWellConfiguration)
				{
					battleArena->ConfigureSoulWells(phaseData.SoulWellsData);
				}			

				// Remove spells by Tag
				for (auto& objectTag : phaseData.SpellsInPlayToRemove)
				{
					TArray<FPlayerData>& playerDefinitions = battleArena->GetPlayerDefinitions();
					for (auto& playerData : playerDefinitions)
					{
						APlayerController* controller = playerData.PlayerControllerActor;
						if (controller)
						{
							ASoulShardsCharacter* caster = Cast<ASoulShardsCharacter>(controller->GetPawn());
							if (caster)
							{
								battleArena->RemoveSpellsByTag(caster, objectTag, ESpellApplyUndoCause::AUCSpellDispelled);
							}
						}
					}
				}

				ABattleArenaPlayerController* playerController = SoulShardsHelper::GetFirstLocalNonReplayPlayerController(this->GetWorld());
				if (playerController)
				{
					playerController->ShowInspectionMode(false);
					
					UBattleArenaHudWidget* hudWidget = playerController->GetHudWidget();
					if (hudWidget)
					{
						if (phaseData.ApplyReminder)
						{
							// Sets the phase reminder
							FText Reminder;
							if (!phaseData.Reminder.IsEmpty())
							{
								hudWidget->EnableReminderEvent(phaseData.Reminder);
							}
							else
							{
								hudWidget->DisableReminderEvent();
							}
						}

						hudWidget->SetTargetObjectName(phaseData.HudTarget);
					}

					ASoulShardsCharacter* caster = Cast<ASoulShardsCharacter>(playerController->GetPawn());
					if (caster)
					{
						//Reset any highlight lock
						const ABattleDeckActor* battleDeckActor = caster->GetBattleDeckActor();
						UTargetSelectionController* targetSelectionController = battleDeckActor->GetTargetSelectionController();
						targetSelectionController->LockHighlight(nullptr);						

						// Destroys all creatures
						if (phaseData.DestroyAllCreatures)
						{
							TArray<ABattleArenaCharacter*> characters;
							battleArena->GetNonSoulCasterCreatures(characters);

							for (auto& itCreatures: characters)
							{
								this->DestroyCreature(itCreatures);
							}
						}

						// Sets player input mask
						UPlayerInputController* inputController = caster->GetInputController();
						if (inputController)
						{
							if (phaseData.ApplyInputMask)
							{
								inputController->SetInputMask(phaseData.InputMask);
							}

							if (phaseData.ApplyInputMask2)
							{
								inputController->SetInputMask2(phaseData.InputMask2);
							}
						}

						// Applies allowed spells
						if (phaseData.ApplyAllowedSpells)
						{
							caster->SetAllowedSpells(phaseData.AllowedSpells);
						}

						// Applies prohibited spells
						if (phaseData.ApplyProhibitedSpells)
						{
							caster->SetProhibitedSpells(phaseData.ProhibitedSpells);
						}

						TD20Character* d20Character = caster->GetD20Character();
						if (d20Character)
						{
							TD20Stat* hitPointsStat = d20Character->GetBaseStats()->GetHitPoints();

							// Heal Caster
							if (phaseData.HealPlayer0)
							{
								{

									TD20Stat* maxHitPointsStat = d20Character->GetBaseStats()->GetMaxHitPoints();
									hitPointsStat->SetPermanentValue(*maxHitPointsStat->GetValue());
								}
							}
						}

						TDuelPlayer* player = battleArena->GetPlayerForCharacter(caster);
						if (player)
						{
							// Shuffle player cards in library
							if (phaseData.ShuffleAllIntoLibrary)
							{
								player->ShuffleAllIntoLibrary(false);
							}

							// Returns removed cards to library
							if (phaseData.ReturnRemovedCardsToLibrary)
							{
								player->ReturnRemovedCardsToLibrary();
							}

							// Removes card from game
							if (phaseData.RemoveCardsFromGame.Num() > 0)
							{
								player->RemoveCardsFromGame(phaseData.RemoveCardsFromGame);
							}

							// Makes sure certain cards in user hand
							if (phaseData.EnsureCardsInHand.Num() > 0)
							{
								player->EnsureCardsInHand(phaseData.EnsureCardsInHand);
								caster->GetBattleDeckActor()->GetSelectedCardSlotController().SetSelectedCardSlotIndex(0);
							}


						}
					}
				}

				// Shows objects
				for (auto& objectTag : phaseData.ObjectsToShow)
				{
					TArray<AActor*> actors;
					UGameplayStatics::GetAllActorsWithTag(this, objectTag, actors);

					for (auto& actor : actors)
					{
						actor->SetActorHiddenInGame(false);
					}
				}

				// Hides objects
				for (auto& objectTag : phaseData.ObjectsToHide)
				{
					TArray<AActor*> actors;
					UGameplayStatics::GetAllActorsWithTag(this, objectTag, actors);

					for (auto& actor : actors)
					{
						actor->SetActorHiddenInGame(true);
					}
				}

				gameMode->SetGameLevelFeatureEnabled(EConfigurableGameLevelFeature::CGFAutoDefenseDome, phaseData.EnableAutoDefenseDome);
				gameMode->SetGameLevelFeatureEnabled(EConfigurableGameLevelFeature::CGFHudCharacterUpdates, phaseData.EnableHudCharacterUpdates);
				gameMode->SetGameLevelFeatureEnabled(EConfigurableGameLevelFeature::CGFCreatureMigration, phaseData.EnableCreatureMigration);
				gameMode->SetGameLevelFeatureEnabled(EConfigurableGameLevelFeature::CGFHudCrossHair, phaseData.EnableCrossHair);
				gameMode->SetGameLevelFeatureEnabled(EConfigurableGameLevelFeature::CGFCreatureDamage, phaseData.EnableCreatureDamage);
			}
		}
	}
}

void UScriptedGamePhase::OnPhaseStart_Implementation()
{
	Super::OnPhaseStart_Implementation();
	this->ExecutePhaseData(this->PhaseStartData);
	
}

void UScriptedGamePhase::SetViewTarget(const FName& cameraTag)
{
	UWorld* world = this->GetWorld();
	if (world)
	{
		AHttpEnabledPlayerController* playerController = Cast<AHttpEnabledPlayerController>(world->GetFirstPlayerController());
		if (playerController)
		{
			playerController->SetViewCamera(cameraTag, NAME_None, 1.0f);
		}
	}
}

ASoulShardsCharacter* UScriptedGamePhase::GetSoulCaster(int playerIndex)
{
	TServerBattleArena* serverBattleArena = this->GetBattleArena();
	TSmartListGroupIteratorTemplate<TDuelPlayer, false> itPlayer(serverBattleArena->players);
	TDuelPlayer* player = itPlayer.At(playerIndex);
	if (player != nullptr)
	{
		return static_cast<ASoulShardsCharacter*>(player->GetSoulCaster());
	}

	return nullptr;
}	

TArray<ABattleArenaCharacter*> UScriptedGamePhase::GetCreaturesByZone(EBattleArenaZoneType zoneType, EPartyFilterType partyFilter)
{
	TServerBattleArena* serverBattleArena = this->GetBattleArena();
	TArray<ABattleArenaCharacter*> creatures;

	TSmartListGroupIteratorTemplate<TDuelPlayer, false> itPlayer(serverBattleArena->players);
	TDuelPlayer* player = itPlayer.At(1);

	serverBattleArena->GetCreaturesInZone(player, zoneType, creatures, partyFilter);
	TArray<ABattleArenaCharacter*> toReturn;
	
	for (auto& itCreatures: creatures)
	{
		toReturn.Add(itCreatures);
	}

	return toReturn;
}

void UScriptedGamePhase::SetAiRulesFromDeck(int playerIndex, FDeckData deck)
{
	ABattleArenaGameMode* gameMode = Cast<ABattleArenaGameMode>(this->GetWorld()->GetAuthGameMode());
	if (gameMode)
	{
		TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(gameMode->GetBattleArena());
		if (battleArena)
		{			
			TArray<FPlayerData>& playerDefinitions =battleArena->GetPlayerDefinitions();
			FPlayerData& playerDefinition = playerDefinitions[playerIndex];
			ASoulShardsCharacter* oldCasterPawn = Cast<ASoulShardsCharacter>(playerDefinition.PlayerControllerActor->GetPawn());
			TSubclassOf<ASoulShardsCharacter> oldCasterClass =oldCasterPawn->GetClass();
			if (oldCasterClass != deck.Caster)
			{
				battleArena->ResetPlayerDeck(playerIndex, deck);
			}
			
			// Sets AI player rules
			TAiEngine* aiEngine = static_cast<TAiEngine*>(battleArena->aiEngine);
			TSmartArrayGroupIteratorTemplate<TAIPlayer, false> it(aiEngine->playerList);
			TAIPlayer* aiPlayer = it.At(playerIndex);
			if (aiPlayer)
			{
				FPlayerData tempPlayerData = gameMode->GetPlayerDefinitions()[playerIndex];
				tempPlayerData.Deck = deck;
				TListGroupTemplate<TPlayerAiRule>* playerAiRules = new TListGroupTemplate<TPlayerAiRule>();
				gameMode->CreateAiRulesFromPlayerData(*battleArena, tempPlayerData, *playerAiRules);
				aiPlayer->SetPlayerAiRules(playerAiRules);
			}
		}
	}
}

void UScriptedGamePhase::SetPlayerDeck(int playerIndex, FDeckData deck)
{
	if (deck.Cards.Num() > 0)
	{
		ABattleArenaGameMode* gameMode = Cast<ABattleArenaGameMode>(this->GetWorld()->GetAuthGameMode());
		if (gameMode)
		{
			TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(gameMode->GetBattleArena());
				if (battleArena)
				{
					battleArena->ResetPlayerDeck(playerIndex, deck);
				}
		}
	}
}

ABattleArenaCharacter* UScriptedGamePhase::ReturnCreatureToGame(FGuid removalId, TSubclassOf<class AFxCollection> fxOverride)
{
	ABattleArenaGameMode* gameMode = Cast<ABattleArenaGameMode>(this->GetWorld()->GetAuthGameMode());
	if (gameMode)
	{
		TServerBattleArena* battleArena = static_cast<TServerBattleArena*>(gameMode->GetBattleArena());
		if (battleArena)
		{
			return battleArena->ReturnCreatureToGame(removalId, fxOverride);
		}
	}

	return nullptr;
}


UScriptedGamePhase::~UScriptedGamePhase()
{
}