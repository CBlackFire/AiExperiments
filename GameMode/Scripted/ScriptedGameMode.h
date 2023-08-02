// <copyright file="ScriptedGameMode.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/BattleArenaGameMode.h"
#include "GameMode/Scripted/TriggeredGamePhase.h"
#include "GameMode/Data/PhaseContainer.h"
#include "ScriptedGameMode.generated.h"

UCLASS()
/// <summary>
/// Home Defense Game Mode
/// </summary>
/// <seealso cref="ABattleArenaGameMode" />
class SOULSHARDS_API AScriptedGameMode : public ABattleArenaGameMode
{
	GENERATED_BODY()

private:

	UPROPERTY(transient, BluePrintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhaseContainer* PhaseContainer;

	UPROPERTY(transient)
	/// <summary>
	/// Game phases
	/// </summary>
	TArray<UTriggeredGamePhase*> TriggerPhases;

	UPROPERTY(transient)
	/// <summary>
	/// Current triggered phase
	/// </summary>
	UTriggeredGamePhase* currentTriggeredPhase;

	/// <summary>
	/// Checks if a phase is waiting to be triggered.
	/// </summary>
	UTriggeredGamePhase* CheckForTriggeringPhase();

protected:

	/// <summary>
	/// Initializes the phases.
	/// </summary>
	virtual void InitPhases(FGameOptionsData gameOptions) override;

	/// <summary>
	/// Gets the phase container
	/// </summary>
	UPhaseContainer* GetPhaseContainer(FGameOptionsData gameOptions);

	/// <summary>
	/// Called when the player controller has finished loading its deck.
	/// </summary>
	/// <param name="playerController">The player controller.</param>
	/// <param name="deck">The deck.</param>
	virtual void OnGetDeckCompleted(APlayerController* playerController, FDeckData deck) override;

	/// <summary>
	/// Called every frame
	/// </summary>
	/// <param name="deltaTime">The delta time.</param>
	virtual void Tick(float deltaTime) override;

public:
	/// <summary>
	/// Initializes a new instance of the <see cref="AScriptedGameMode"/> class.
	/// </summary>
	AScriptedGameMode();

	/// <summary>
	/// Adds additional player definitions for the specified game.
	/// </summary>
	virtual void AddPlayerDefinitionsForAI(FPlayerData& vsPlayer, APlayerController* mainPlayerController) override;

	/// <summary>
	/// Gets the game result.
	/// </summary>
	virtual EGameResult GetGameResult(
		int& player0Hp, 
		int& player1Hp,
		EAlternativeWinCondition& player0WinCondition,
		EAlternativeWinCondition& player1WinCondition) override;

	virtual bool ShouldRecordReplay() override;

	/// <summary>
	/// Triggers an event that will be handled by the current phase
	/// </summary>
	virtual void TriggerPhaseEvent(FString eventName, UObject* target = nullptr);
};
