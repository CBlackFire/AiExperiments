// <copyright file="DedicatedServerGameMode.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsGameMode.h"
#include "DedicatedServerGameMode.generated.h"

UCLASS()
/// <summary>
/// Game mode used for the dedicated server.  It manages the logic of waiting for players to start a match
/// </summary>
/// <seealso cref="ASoulShardsGameMode" />
class SOULSHARDS_API ADedicatedServerGameMode : public ASoulShardsGameMode
{
	GENERATED_BODY()

protected:


public:

	ADedicatedServerGameMode();
	
};
