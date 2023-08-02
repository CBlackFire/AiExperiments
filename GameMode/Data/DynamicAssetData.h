// <copyright file="DynamicAssetData.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsEnums.h"
#include "DynamicAssetData.generated.h"


USTRUCT(BlueprintType)
/// <summary>
/// Data to dynamically locate an asset
/// </summary>
struct SOULSHARDS_API FDynamicAssetData
{
	GENERATED_USTRUCT_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FDynamicAssetData)
	/// <summary>
	/// Asset where the map is located
	/// </summary>
	bool IsClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FDynamicAssetData)
	/// <summary>
	/// A soft reference to the asset
	/// </summary>
	FSoftObjectPath AssetReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FDynamicAssetData)
	/// <summary>
	/// Asset class.
	/// </summary>
	UClass* AssetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FDynamicAssetData)
	/// <summary>
	/// A soft reference to the class.
	/// </summary>
	TSoftClassPtr<UObject> ClassReference;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FDynamicAssetData)
	/// <summary>
	/// Realm for this map
	/// </summary>
	EGameRealm AssetRealm;

	/// <summary>
	/// Gets the asset class.
	/// </summary>
	/// <returns>The class of the asset</returns>
	UClass* GetAssetClass();

	/// <summary>
	/// Gets the asset.
	/// </summary>
	/// <returns>The asset</returns>
	UObject* GetAsset();
};
