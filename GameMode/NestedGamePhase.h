// <copyright file="NestedGamePhase.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/BattleArenaGamePhase.h"
#include "NestedGamePhase.generated.h"

UCLASS(EditInlineNew, Blueprintable, BlueprintType)
/// <summary>
/// Represents a phase for a battle arena game
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API UNestedGamePhase : public UBattleArenaGamePhase
{
	GENERATED_BODY()
	
private:

	/// <summary>
	/// The current nested phase index
	/// </summary>
	int currentNestedPhaseIndex;

	/// <summary>
	/// Gets the current nested phase.
	/// </summary>
	/// <returns></returns>
	UBattleArenaGamePhase* GetCurrentNestedPhase();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = UNestedGamePhase)
	/// <summary>
	/// Array with all the nexted phases
	/// </summary>
	TArray<UBattleArenaGamePhase*> NestedPhaseArray;

	/// <summary>
	/// Initializes a new instance of the <see cref="UNestedGamePhase"/> class.
	/// </summary>
	UNestedGamePhase();
	
	/// <summary>
	/// Checks the phase end condition.
	/// </summary>
	/// <returns>
	/// Returns true if the phase should end.  False Otherwise
	/// </returns>
	virtual bool CheckPhaseEndCondition_Implementation() override;

	/// <summary>
	/// Checks the game end condition.
	/// </summary>
	/// <param name="battleArena">The battle arena.</param>
	/// <returns>
	/// Returns true if the game should end.  False Otherwise
	/// </returns>
	virtual bool CheckGameEndCondition_Implementation() override;

	/// <summary>
	/// Called when an event is triggered
	/// </summary>
	virtual void OnEventTriggered_Implementation(const FString& eventName, UObject* target = nullptr);

	/// <summary>
	/// Starts the phase for a game instance
	/// </summary>
	virtual void StartPhase() override;

	/// <summary>
	/// Ends the phase for a game instance
	/// </summary>
	virtual void EndPhase() override;
};
