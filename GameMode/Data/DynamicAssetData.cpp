// <copyright file="DynamicAssetData.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "DynamicAssetData.h"

UClass* FDynamicAssetData::GetAssetClass()
{
	if (this->IsClass)
	{
		if (this->AssetClass != nullptr)
		{
			return this->AssetClass;
		} else if (!this->ClassReference.IsNull())
		{
			return this->ClassReference.LoadSynchronous();
		}
	}
	else if (this->AssetReference.IsValid())
	{
		return this->AssetReference.TryLoad()->GetClass();
	}

	return nullptr;
}


UObject* FDynamicAssetData::GetAsset()
{
	if (!this->IsClass)
	{
		return this->AssetReference.TryLoad();
	}

	return nullptr;
}