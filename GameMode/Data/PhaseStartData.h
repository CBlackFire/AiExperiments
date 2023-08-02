// <copyright file="PhaseStartData.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsEnums.h"
#include "Player/Data/DeckPhaseData.h"
#include "Player/Data/DeckSoulWellsData.h"
#include "PhaseStartData.generated.h"


USTRUCT(BlueprintType)
/// <summary>
/// Data for a player
/// </summary>
struct SOULSHARDS_API FPhaseStartData 
{
	GENERATED_USTRUCT_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AIConfiguration)
	/// <summary>
	/// Indicates whether phase data will be ignored altogether. 
	/// </summary>
	bool IgnorePhaseData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraConfiguration)
	/// <summary>
	/// Asset where the map is located
	/// </summary>
	FName StartCameraTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AIConfiguration)
	/// <summary>
	/// Whether the AI will require mana when casting 
	/// </summary>
	bool RequiresManaToCast;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AIConfiguration)
	/// <summary>
	/// Ai Rules Configuration 
	/// </summary>
	FDeckPhaseData PhaseData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ArenaConfiguration)
	/// <summary>
	/// Configuration data for soulwells in this deck.
	/// </summary>	
	FDeckSoulWellsData SoulWellsData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputConfiguration)
	/// <summary>
	/// Whether to apply the caster input mask below.
	/// </summary>
	bool ApplyInputMask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputConfiguration)
	/// <summary>
	/// Whether to apply the caster input mask below.
	/// </summary>
	bool ApplyInputMask2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputConfiguration, Meta = (Bitmask, BitmaskEnum = "EPlayerInputMask"))
	/// <summary>
	/// Input to be allowed/blocked
	/// </summary>
	int InputMask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputConfiguration, Meta = (Bitmask, BitmaskEnum = "EPlayerInputMask2"))
	/// <summary>
	/// Input to be allowed blocked
	/// </summary>
	int InputMask2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ArenaConfiguration)
	/// <summary>
	/// Whether to apply the auto advance configuration
	/// </summary>
	bool ApplyAutoAdvanceConfiguration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ArenaConfiguration)
	/// <summary>
	/// Auto Advance Configuration
	/// </summary>
	TArray<bool> AutoAdvanceConfiguration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ArenaConfiguration)
	/// <summary>
	/// Array of objects to show
	/// </summary>
	TArray<FName> ObjectsToShow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ArenaConfiguration)
	/// <summary>
	/// Array of objects to hide
	/// </summary>
	TArray<FName> ObjectsToHide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpellConfiguration)
	/// <summary>
	/// Whether to apply allowed spells
	/// </summary>
	bool ApplyAllowedSpells;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpellConfiguration)
	/// <summary>
	/// Array of spells allowed to be cast during the pase
	/// </summary>
	TArray<FName> AllowedSpells;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpellConfiguration)
	/// <summary>
	/// Whether to apply allowed spells
	/// </summary>
	bool ApplyProhibitedSpells;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpellConfiguration)
	/// <summary>
	/// Array of spells allowed to be cast during the pase
	/// </summary>
	TArray<FName> ProhibitedSpells;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpellConfiguration)
	/// <summary>
	/// Array of tags of spells in play that should be removed
	/// </summary>
	TArray<FName> SpellsInPlayToRemove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerCardsConfiguration)
	/// <summary>
	/// Indicates whether the all cards should be shuffled back into the library
	/// </summary>
	bool ShuffleAllIntoLibrary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameActions)
	/// <summary>
	/// Whether to heal player0
	/// </summary>
	bool HealPlayer0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerCardsConfiguration)
	/// <summary>
	/// Makes sure the cards in the array are in hand at the beginning of the phase (this cards must already be in the deck.
	/// </summary>
	TArray<FName> EnsureCardsInHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerCardsConfiguration)
	/// <summary>
	/// Makes sure the cards in the array are in hand at the beginning of the phase (this cards must already be in the deck.
	/// </summary>
	TArray<FName> RemoveCardsFromGame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerCardsConfiguration)
	/// <summary>
	/// Indicates whether the all cards should be shuffled back into the library
	/// </summary>
	bool ReturnRemovedCardsToLibrary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ArenaConfiguration)
	/// <summary>
	/// If true destroys all creatures in the board
	/// </summary>
	bool DestroyAllCreatures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TutorialConfiguration)
	/// <summary>
	/// If true sets the reminder
	/// </summary>
	bool ApplyReminder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ArenaConfiguration)
	/// <summary>
	/// If true auto defense domes are enabled.
	/// </summary>
	bool EnableAutoDefenseDome;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TutorialConfiguration)
	/// <summary>
	/// If not empty shows a reminder for the phase
	/// </summary>
	FText Reminder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TutorialConfiguration)
	/// <summary>
	/// If not empty marks an object as HUD target
	/// </summary>
	FName HudTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FeatureConfiguration)
    /// <summary>
    /// If true, character portraits are added to the Hud.
    /// </summary>
    bool EnableHudCharacterUpdates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FeatureConfiguration)
	/// <summary>
	/// If true, enables creature migration.
	/// </summary>
	bool EnableCreatureMigration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpellConfiguration)
	/// <summary>
	/// Whether to enable cross hair.
	/// </summary>
	bool EnableCrossHair;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpellConfiguration)
	/// <summary>
	/// Whether to enable cross hair.
	/// </summary>
	bool EnableCreatureDamage;

	FPhaseStartData();
};