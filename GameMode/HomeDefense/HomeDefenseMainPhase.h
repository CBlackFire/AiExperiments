// <copyright file="HomeDefenseMainPhase.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/TimeLimitGamePhase.h"
#include "HomeDefenseMainPhase.generated.h"

UCLASS(Config = Game)
/// <summary>
/// Defines the main phase for a duel game type
/// </summary>
class SOULSHARDS_API UHomeDefenseMainPhase : public UTimeLimitGamePhase
{
	GENERATED_BODY()
	
private:

	UPROPERTY(Config)
	/// <summary>
	/// Defines the duration of the invasion phase.
	/// </summary>
	int InvasionPhaseDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UHomeDefenseMainPhase, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Defines the handicap multiplier.
	/// </summary>
	float HandicapMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UHomeDefenseMainPhase, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Defines the handicap multiplier.
	/// </summary>
	bool CheckHitPointsEndPhaseCondition;


public:
	/// <summary>
	/// Initializes a new instance of the <see cref="THomeDefenseMainPhase"/> class.
	/// </summary>
	UHomeDefenseMainPhase();

	/// <summary>
	/// Returns the phase duration in seconds
	/// </summary>
	virtual int GetPhaseDuration() override;

	/// <summary>
	/// Sets the handicapMultiplier
	/// </summary>
	void SetHandicapMultiplier(float handicapMultiplier);

	/// <summary>
	/// Checks the phase end condition.
	/// </summary>
	/// <returns>
	/// Returns true if the phase should end.  False Otherwise
	/// </returns>
	bool CheckPhaseEndCondition_Implementation() override;

	/// <summary>
	/// Finalizes an instance of the <see cref="THomeDefenseMainPhase"/> class.
	/// </summary>
	virtual ~UHomeDefenseMainPhase();
};
