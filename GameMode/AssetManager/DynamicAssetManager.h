// <copyright file="DynamicAssetManager.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "DynamicAssetManager.generated.h"

UCLASS(BlueprintType)
/// <summary>
/// Class that dynamically loads assets
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API UDynamicAssetManager : public UObject
{
	GENERATED_BODY()
	
private:

	UPROPERTY()
	/// <summary>
	/// Array containing classes of assets
	/// </summary>
	TMap<FName, UClass*> loadedClasses;

	UPROPERTY()
	/// <summary>
	/// Array containing assets
	/// </summary>
	TMap<FName, UObject*> loadedAssets;

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="UDynamicAssetManager"/> class.
	/// </summary>
	UDynamicAssetManager();

	UFUNCTION(BlueprintCallable, Category = UDynamicAssetManager)
	/// <summary>
	/// Maps blueprint asset classes 
	/// </summary>
	/// <param name="paths">The paths.</param>
	/// <param name="">The class of the assets</param>
	void MapBlueprintClasses(TArray<FString> paths, UClass* assetClass);

	/// <summary>
	/// Maps assets
	/// </summary>
	/// <param name="paths">The paths.</param>
	/// <param name="">The class of the assets</param>
	void MapAssets(TArray<FString> paths, UClass* assetClass);

	/// <summary>
	/// Maps the assets of the given class that exist in the given paths
	/// </summary>
	virtual UWorld* GetWorld() const override;

	/// <summary>
	/// Spawns the actor.
	/// </summary>
	/// <param name="className">Name of the class.</param>
	/// <returns></returns>
	AActor* SpawnActor(FName className);

	/// <summary>
	/// Spawns the actor.
	/// </summary>
	/// <param name="className">Name of the class.</param>
	/// <returns></returns>
	AActor* SpawnActorDeferred(FName className);

	/// <summary>
	/// Creates a new object
	/// </summary>
	/// <param name="className">Name of the class.</param>
	/// <param name="outer">The outer object.</param>
	/// <returns></returns>
	UObject* CreateNewObject(FName className, UObject* outer, FString name);

	/// <summary>
	/// Returns true if the class exists 
	/// </summary>
	/// <param name="className">Name of the class.</param>
	/// <returns></returns>
	bool Exists(FName className);

	UFUNCTION(BlueprintCallable, Category = UDynamicAssetManager)
	/// <summary>
	/// Finds the class.
	/// </summary>
	/// <param name="className">Name of the class.</param>
	/// <returns></returns>
	UClass* FindClass(FName className);

	/// <summary>
	/// Finds the asset.
	/// </summary>
	/// <param name="className">Name of the asset.</param>
	/// <returns></returns>
	UObject* FindAsset(FName assetName);
};
