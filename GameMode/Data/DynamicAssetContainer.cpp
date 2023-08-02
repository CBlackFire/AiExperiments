// <copyright file="DynamicAssetContainer.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>


#include "SoulShards.h"
#include "DynamicAssetContainer.h"

UDynamicAssetContainer::UDynamicAssetContainer()
{
	this->GenerateSoftReferencesOnSave = false;
}

TMap<FName, FDynamicAssetData>& UDynamicAssetContainer::GetAssetMap()
{
	return this->AssetMap;
}

UClass* UDynamicAssetContainer::GetAssetClass(FName assetName)
{
	FDynamicAssetData* asset = AssetMap.Find(assetName);
	if (asset)
	{
		check(asset->IsClass);
		return asset->GetAssetClass();
	}
	return nullptr;
}

UObject* UDynamicAssetContainer::GetAsset(FName assetName)
{
	FDynamicAssetData* asset = AssetMap.Find(assetName);
	if (asset)
	{
		return asset->GetAsset();
	}
	return nullptr;
}

void UDynamicAssetContainer::AddFrom(UDynamicAssetContainer* other)
{
	TMap<FName, FDynamicAssetData>& otherMap = other->GetAssetMap();
	for (auto& item : otherMap)
	{
		if (!this->AssetMap.Find(item.Key))
		{
			this->AssetMap.Add(item.Key, item.Value);
		}
	}
}

void UDynamicAssetContainer::PreSave(const ITargetPlatform* TargetPlatform)
{
	Super::PreSave(TargetPlatform);
	
	if (this->GenerateSoftReferencesOnSave)
	{
		for (auto& item : this->AssetMap)
		{
			if (item.Value.IsClass && item.Value.AssetClass != nullptr)
			{
				item.Value.ClassReference = item.Value.AssetClass;
				item.Value.AssetClass = nullptr;				
			}
		}
	}
}