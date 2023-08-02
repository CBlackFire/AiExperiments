// <copyright file="BattleArenaGamePhase.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "BattleArenaGamePhase.generated.h"

UCLASS(EditInlineNew, Blueprintable, BlueprintType)
/// <summary>
/// Represents a phase for a battle arena game
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API UBattleArenaGamePhase : public UObject
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UBattleArenaGamePhase, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Whether this phase is enabled.  Disabled phases will be ignored.
	/// </summary>
	bool Enabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UBattleArenaGamePhase, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Phase name.
	/// </summary>
	FString PhaseName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UBattleArenaGamePhase, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Phase goal.
	/// </summary>
	FString PhaseGoalText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UBattleArenaGamePhase, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Whether to allow creature summons to be queued.
	/// </summary>
	bool AllowSummonQueueing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UBattleArenaGamePhase, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Whether to allow creatures to drop items when dying.
	/// </summary>
	bool AllowCreatureDrops;

	/// <summary>
	/// The phase index
	/// </summary>
	int phaseIndex;

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="UBattleArenaGamePhase"/> class.
	/// </summary>
	UBattleArenaGamePhase();
	
	/// <summary>
	/// Gets the world
	/// </summary>
	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = UBattleArenaGamePhase)
	/// <summary>
	/// Checks the phase end condition.
	/// </summary>
	/// <returns>
	/// Returns true if the phase should end.  False Otherwise
	/// </returns>
	bool CheckPhaseEndCondition();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = UBattleArenaGamePhase)
	/// <summary>
	/// Checks the game end condition.
	/// </summary>
	/// <param name="battleArena">The battle arena.</param>
	/// <returns>
	/// Returns true if the game should end.  False Otherwise
	/// </returns>
	bool CheckGameEndCondition();

	UFUNCTION(BlueprintNativeEvent, Category = UBattleArenaGamePhase)
	/// <summary>
	/// Called when an event is triggered
	/// </summary>
	void OnEventTriggered(const FString& eventName, UObject* target = nullptr);

	UFUNCTION(BlueprintNativeEvent, Category = UBattleArenaGamePhase)
	/// <summary>
	/// Called when the phase starts
	/// </summary>
	void OnPhaseStart();

	UFUNCTION(BlueprintImplementableEvent, Category = UBattleArenaGamePhase)
	/// <summary>
	/// Called when the phase starts
	/// </summary>
	void OnPhaseEnd();

	UFUNCTION(BlueprintCallable, Category = UBattleArenaGamePhase)
	/// <summary>
	/// Triggers an event that will be handled by the current phase
	/// </summary>
	void TriggerPhaseEvent(const FString& eventName, UObject* target = nullptr);

	UFUNCTION(BlueprintCallable, Category = UBattleArenaGamePhase)
	/// <summary>
	/// Starts the phase for a game instance
	/// </summary>
	virtual void StartPhase();

	UFUNCTION(BlueprintCallable, Category = UBattleArenaGamePhase)
	/// <summary>
	/// Ends the phase for a game instance
	/// </summary>
	/// <param name="battleArena">The battle arena.</param>
	virtual void EndPhase();	

	/// <summary>
	/// Gets the name of the phase.
	/// </summary>
	/// <returns></returns>
	FString GetPhaseName() const;

	/// <summary>
	/// Gets the phase goal text.
	/// </summary>
	/// <returns></returns>
	FString GetPhaseGoalText() const;

	/// <summary>
	/// Gets the index of the phase.
	/// </summary>
	/// <returns></returns>
	int GetPhaseIndex() const;

	/// <summary>
	/// Gets whether this phase is enabled
	/// </summary>
	bool GetIsEnabled() const;

	/// <summary>
	/// Sets the index of the phase.
	/// </summary>
	/// <param name="phaseIndex">Index of the phase.</param>
	void SetPhaseIndex(int phaseIndex);

	/// <summary>
	/// Sets the name of the phase.
	/// </summary>
	/// <param name="phaseName">Name of the phase.</param>
	void SetPhaseName(FString phaseName);

	/// <summary>
	/// Sets the phase goal text.
	/// </summary>
	/// <param name="phaseGoalText">Name of the phase.</param>
	void SetPhaseGoalText(FString phaseGoalText);

	/// <summary>
	/// Gets whether summon queueing is allowed.
	/// </summary>
	bool GetAllowSummonQueueing();

	/// <summary>
	/// Gets whether to allow creatures to drpo items when dying.
	/// </summary>
	bool GetAllowCreatureDrops();

	/// <summary>
	/// Gets the battle arena associated to this phase.
	/// </summary>
	class TServerBattleArena* GetBattleArena();
};
