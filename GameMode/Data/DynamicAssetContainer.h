// <copyright file="DynamicAssetContainer.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsEnums.h"
#include "GameMode/Data/DynamicAssetData.h"
#include "DynamicAssetContainer.generated.h"

UCLASS(Blueprintable, BlueprintType)
/// <summary>
/// Class that contains information about asset location an name
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API UDynamicAssetContainer : public UObject
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UDynamicAssetContainer, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// If true, soft references are generated for each class reference on Save.  Class references are cleaned up.
	/// </summary>
	bool GenerateSoftReferencesOnSave;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UDynamicAssetContainer, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Map with a set of arrays
	/// </summary>
	TMap<FName, FDynamicAssetData> AssetMap;

protected:

	/// <summary>
	/// Called before save to generate soft references.
	/// </summary>
	virtual void PreSave(const class ITargetPlatform* TargetPlatform) override;

public:

	/// <summary>
	/// Initializes a new instance of the type UDynamicAssetContainer.  
	/// </summary>
	UDynamicAssetContainer();

	/// <summary>
	/// Gets the asset map.
	/// </summary>
	/// <returns>The asset map.</returns>
	TMap<FName, FDynamicAssetData>& GetAssetMap();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = UDynamicAssetContainer)
	/// <summary>
	/// Gets an asset class by Name.
	/// </summary>
	/// <param name="assetName">Name of the asset.</param>
	/// <returns>The asset</returns>
	UClass* GetAssetClass(FName assetName);

	/// <summary>
	/// Gets an asset by Name.
	/// </summary>
	/// <param name="assetName">Name of the asset.</param>
	/// <returns>The asset</returns>
	UObject* GetAsset(FName assetName);

	/// <summary>
	/// Adds assets from another container.
	/// </summary>
	/// <param name="other">The other container.</param>
	void AddFrom(UDynamicAssetContainer* other);
};
