// <copyright file="PhaseContainer.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsEnums.h"
#include "GameMode/BattleArenaGamePhase.h"
#include "GameMode/Scripted/TriggeredGamePhase.h"
#include "PhaseContainer.generated.h"

UCLASS(Blueprintable, BlueprintType)
/// <summary>
/// Class that contains information about playable maps
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API UPhaseContainer : public UObject
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = UPhaseContainer, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Array with all the game phases
	/// </summary>
	TArray<UBattleArenaGamePhase*> PhasesArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = UPhaseContainer, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Array with all the game phases
	/// </summary>
	TArray<UTriggeredGamePhase*> TriggerPhasesArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UPhaseContainer, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Reference to the map used by this phase group
	/// </summary>
	FSoftObjectPath MapReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UPhaseContainer, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Reference to a content deck that can be loaded for the game.
	/// </summary>
	FString ContentDeckId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UPhaseContainer, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Phase index that the loaded content will apply to
	/// </summary>
	int ContenDeckStartPhaseIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UPhaseContainer, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Name of the player
	/// </summary>
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UPhaseContainer, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Indicates if this phase container is for a survival game
	/// </summary>
	bool IsSurvival;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UPhaseContainer, meta = (AllowPrivateAccess = "true"))
    /// <summary>
    /// Indicates if replay record should be forced independently of the predefined setting in the GameMode.
    /// </summary>
    bool ForceReplayRecord;	

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="UPhaseContainer"/> class.
	/// </summary>
	UPhaseContainer();

	/// <summary>
	/// Gets the phase array.
	/// </summary>
	/// <returns></returns>
	TArray<UBattleArenaGamePhase*>& GetPhaseArray();

	/// <summary>
	/// Gets the trigger phase array.
	/// </summary>
	/// <returns></returns>
	TArray<UTriggeredGamePhase*>& GetTriggerPhaseArray();

	/// <summary>
	/// Gets a reference to a content deck that can be loaded for the game.
	/// </summary>
	FString GetContentDeckId() const;

	/// <summary>
	/// Gets the phase index that the loaded content will apply to
	/// </summary>
	int GetContenDeckStartPhaseIndex() const;

	/// <summary>
	/// Gets the player name
	/// </summary>
	FString GetPlayerName() const;

	/// <summary>
	/// Gets a flag indicating whether replay recording should be forced
	/// </summary>
	const bool GetForceReplayRecord();

	/// <summary>
	/// Gets if this is from a survival game
	/// </summary>
	bool GetIsSurvival() const;
};

