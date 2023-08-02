// <copyright file="TimeLimitGamePhase.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/BattleArenaGamePhase.h"
#include "TimeLimitGamePhase.generated.h"

UCLASS()
/// <summary>
/// Defines the main phase for a duel game type
/// </summary>
class SOULSHARDS_API UTimeLimitGamePhase : public UBattleArenaGamePhase
{
	GENERATED_BODY()
	
private:

	/// <summary>
	/// The phase start time
	/// </summary>
	float phaseStartTime;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UTimeLimitGamePhase, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Data that indicates the desired state of the game at the beginning of the game.
	/// </summary>
	bool EnablePhaseNotification;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UTimeLimitGamePhase, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Indicates if time should be shown
	/// </summary>
	bool ShowTimer;

public:
	/// <summary>
	/// Initializes a new instance of the <see cref="TTimeLimitGamePhase"/> class.
	/// </summary>
	UTimeLimitGamePhase();

	/// <summary>
	/// Checks the phase end condition.
	/// </summary>
	/// <returns>
	/// Returns true if the phase should end.  False Otherwise
	/// </returns>
	 bool CheckPhaseEndCondition_Implementation() override;

	/// <summary>
	/// Checks the game end condition.
	/// </summary>
	/// <returns>
	/// Returns true if the game should end.  False Otherwise
	/// </returns>
	virtual bool CheckGameEndCondition_Implementation() override;

	/// <summary>
	/// Starts the phase for a game instance
	/// </summary>
	virtual void StartPhase();

	/// <summary>
	/// Ends the phase for a game instance
	/// </summary>
	virtual void EndPhase();
	
	/// <summary>
	/// Returns the phase duration in seconds
	/// </summary>
	virtual int GetPhaseDuration();

	/// <summary>
	/// Finalizes an instance of the <see cref="TTimeLimitGamePhase"/> class.
	/// </summary>
	virtual ~UTimeLimitGamePhase();
};
