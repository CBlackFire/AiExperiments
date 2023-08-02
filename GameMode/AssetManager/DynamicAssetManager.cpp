// <copyright file="DynamicAssetManager.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>


#include "SoulShards.h"
#include "ARFilter.h"
#include "DynamicAssetManager.h"

UDynamicAssetManager::UDynamicAssetManager()
{

}

void UDynamicAssetManager::MapAssets(TArray<FString> paths, UClass* assetClass)
{
	FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FString> scanPaths;
	for (auto& scanPath : paths)
	{
		scanPaths.Add(scanPath + TEXT("/"));
	}

	assetRegistryModule.Get().ScanPathsSynchronous(scanPaths);

	FARFilter filter;
	filter.bRecursivePaths = true;
	filter.bRecursiveClasses = true;
	filter.ClassNames.Add(assetClass->GetFName());
	for (auto& path : paths)
	{
		filter.PackagePaths.Add(FName(*path));
	}

	TArray<FAssetData> assets;
	assetRegistryModule.Get().GetAssets(filter,  assets);

	for (auto& asset : assets)
	{
		loadedAssets.Add(asset.AssetName, asset.GetAsset());
	}
}

void UDynamicAssetManager::MapBlueprintClasses(TArray<FString> paths, UClass* assetClass)
{
	FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FString> scanPaths;
	for (auto& scanPath : paths)
	{
		scanPaths.Add(scanPath + TEXT("/"));
	}

	assetRegistryModule.Get().ScanPathsSynchronous(scanPaths);

	FARFilter filter;
	filter.bRecursivePaths = true;
	filter.bRecursiveClasses = true;
	filter.ClassNames.Add((UBlueprint::StaticClass()->GetFName()));
	for (auto& path : paths)
	{
		filter.PackagePaths.Add(FName(*path));
	}

	TArray<FAssetData> assets;
	assetRegistryModule.Get().GetAssets(filter, assets);
	
	for (auto& asset : assets)
	{
		//try 
		//{
		// This happens while we are initializing the game mode, so any attemp to load game mode assets would fail
			if (!asset.AssetName.ToString().StartsWith("GM_"))
			{
				UBlueprint* bp = Cast<UBlueprint>(asset.GetAsset());
				if (bp)
				{
					auto gc = bp->GeneratedClass;
					if (gc)
					{
						if (gc->IsChildOf(assetClass))
						{
							this->loadedClasses.Add(FName(*gc->GetName()), gc);
						}
					}
				}
				else
				{
					auto generatedClassName = (asset.AssetName.ToString() + "_C");

					UClass* clazz = FindObject<UClass>(asset.GetPackage(), *generatedClassName);
					if (clazz)
					{
						if (clazz->IsChildOf(assetClass))
						{
							this->loadedClasses.Add(FName(*clazz->GetName()), clazz);
						}
					}
					else
					{
						UObjectRedirector* renamedClassRedirector = FindObject<UObjectRedirector>(asset.GetPackage(), *generatedClassName);
						if (renamedClassRedirector)
						{
							UClass* gc = CastChecked<UClass>(renamedClassRedirector->DestinationObject);
							if (gc->IsChildOf(assetClass))
							{
								this->loadedClasses.Add(FName(*gc->GetName()), gc);
							}
						}
					}

				}
			}
		//}
		//catch (...)
		//{

		//}
	}
}

AActor* UDynamicAssetManager::SpawnActor(FName className)
{
	UClass* loadedClass = this->FindClass(className);
	if (loadedClass)
	{
		FActorSpawnParameters spawnParameters;
		spawnParameters.bNoFail = true;

		AActor* actor = this->GetWorld()->SpawnActor<AActor>(loadedClass, FTransform::Identity, spawnParameters);
		check(actor);
		return actor;
	}
	return nullptr;
}

AActor* UDynamicAssetManager::SpawnActorDeferred(FName className)
{
	UClass* loadedClass = this->FindClass(className);
	if (loadedClass)
	{
		AActor* actor = this->GetWorld()->SpawnActorDeferred<AActor>(loadedClass, FTransform::Identity);
		check(actor);
		return actor;
	}
	return nullptr;
}


UObject* UDynamicAssetManager::CreateNewObject(FName className, UObject* outer, FString name)
{
	UClass* loadedClass = this->FindClass(className);
	if (loadedClass)
	{
		FActorSpawnParameters spawnParameters;
		spawnParameters.bNoFail = true;

		UObject* object = NewObject<UObject>(outer, loadedClass, *name);
		check(object);
		return object;
	}

	return nullptr;
}

UWorld* UDynamicAssetManager::GetWorld() const
{
	// The CDO doesn't have a World
	return HasAnyFlags(RF_ClassDefaultObject) ? nullptr : this->GetOuter()->GetWorld();
}

UClass* UDynamicAssetManager::FindClass(FName className)
{
	FString classNameString = className.ToString();
	if (!classNameString.EndsWith("_C"))
	{
		classNameString = classNameString + "_C";
		className = *classNameString;
	}

	UClass** classPtr = loadedClasses.Find(className);
	if (classPtr != nullptr)
	{
		return *classPtr;
	}
	return nullptr;
}

bool UDynamicAssetManager::Exists(FName className)
{
	return this->FindClass(className) != nullptr;
}

UObject* UDynamicAssetManager::FindAsset(FName assetName)
{
	UObject** assetPtr = loadedAssets.Find(assetName);
	if (assetPtr != nullptr)
	{
		return *assetPtr;
	}
	return nullptr;
}