// <copyright file="DeckContainer.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsEnums.h"
#include "Player/Data/DeckData.h"
#include "DeckContainer.generated.h"

UCLASS(Blueprintable, BlueprintType)
/// <summary>
/// Class that contains information about pre-defined decks
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API UDeckContainer : public UObject
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UDeckContainer, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Array with all the pre-defined decks
	/// </summary>
	TMap<FString, FDeckData> DecksMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UDynamicAssetContainer, meta = (AllowPrivateAccess = "true"))
    /// <summary>
    /// If true, soft references are generated for each class reference on Save.  Class references are cleaned up.
    /// </summary>
    bool GenerateSoftReferencesOnSave;

protected:

	/// <summary>
    /// Called before save to generate soft references.
    /// </summary>
    virtual void PreSave(const class ITargetPlatform* TargetPlatform) override;
	
public:

	/// <summary>
	/// Initializes a new instance of the UDeckContainer type. 
	/// </summary>
	UDeckContainer();

	FDeckData* GetDeckById(FString deckId);
};
