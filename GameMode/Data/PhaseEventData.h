// <copyright file="PhaseEventData.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "PhaseEventData.generated.h"


USTRUCT(BlueprintType)
/// <summary>
/// Data for the spawn queue
/// </summary>
struct SOULSHARDS_API FPhaseEventData
{
	GENERATED_USTRUCT_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FPhaseEventData)
	/// <summary>
	/// Game phase that was current when the event was triggered
	/// </summary>
	UBattleArenaGamePhase* GamePhase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FPhaseEventData)
	/// <summary>
	/// The target parameter of the event
	/// </summary>
	UObject* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FPhaseEventData)
	/// <summary>
	/// The EventName parameter of the event
	/// </summary>
	FString EventName;

	FPhaseEventData();
	FPhaseEventData(UBattleArenaGamePhase* gamePhase, UObject* target, FString eventName);
	
	bool operator==( const FPhaseEventData& other ) const
	{
		return this->GamePhase == other.GamePhase &&
			this->Target == other.Target && this->EventName == other.EventName;			
	}
};
