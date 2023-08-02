// <copyright file="DeckContainer.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>


#include "SoulShards.h"
#include "DeckContainer.h"

UDeckContainer::UDeckContainer()
{
	this->GenerateSoftReferencesOnSave = false;
}

FDeckData* UDeckContainer::GetDeckById(FString DeckId)
{
	return this->DecksMap.Find(DeckId);
}

void UDeckContainer::PreSave(const class ITargetPlatform* TargetPlatform)
{
	Super::PreSave(TargetPlatform);
	
	if (this->GenerateSoftReferencesOnSave)
	{
		for (auto& item : this->DecksMap)
		{
			FDeckData& deck = item.Value;
			deck.RemoveHardReferences();			
		}
	}
}

