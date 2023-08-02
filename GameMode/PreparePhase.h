// <copyright file="PreparePhase.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/TimeLimitGamePhase.h"
#include "PreparePhase.generated.h"

UCLASS(Config = Game)
/// <summary>
/// Defines the main phase for a duel game type
/// </summary>
class SOULSHARDS_API UPreparePhase : public UTimeLimitGamePhase
{
	GENERATED_BODY()
	
private:

	UPROPERTY(Config)
	/// <summary>
	/// Defines the duration of the preparation phase.
	/// </summary>
	int PreparationPhaseDuration;

public:
	/// <summary>
	/// Initializes a new instance of the <see cref="TPreparePhase"/> class.
	/// </summary>
	UPreparePhase();

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
	virtual int GetPhaseDuration() override;

	/// <summary>
	/// Finalizes an instance of the <see cref="TPreparePhase"/> class.
	/// </summary>
	virtual ~UPreparePhase();
};
