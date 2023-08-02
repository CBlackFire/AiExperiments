// <copyright file="HomeDefensePreparePhase.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/PreparePhase.h"
#include "HomeDefensePreparePhase.generated.h"

UCLASS(Config = Game)
/// <summary>
/// Defines the main phase for a home defense game type
/// </summary>
class SOULSHARDS_API UHomeDefensePreparePhase : public UPreparePhase
{
	GENERATED_BODY()
	
private:

public:
	/// <summary>
	/// Initializes a new instance of the <see cref="UHomeDefensePreparePhase"/> class.
	/// </summary>
	UHomeDefensePreparePhase();

	/// <summary>
	/// Ends the phase for a game instance
	/// </summary>
	virtual void EndPhase();

	/// <summary>
	/// Finalizes an instance of the <see cref="UHomeDefensePreparePhase"/> class.
	/// </summary>
	virtual ~UHomeDefensePreparePhase();
};
