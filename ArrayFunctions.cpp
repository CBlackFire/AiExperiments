// <copyright file="ArrayFunctions.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShardsBase.h"
#include "ArrayFunctions.h"

void UArrayFunctions::GenericArray_Compact(const void* TargetArray, const FArrayProperty* ArrayProp)
{
	if (TargetArray)
	{
		FScriptArrayHelper ArrayHelper(ArrayProp, TargetArray);
		TArray<int> toDelete;

		for (int i = 0; i < ArrayHelper.Num(); i++)
		{
			if (ArrayHelper.GetRawPtr(i) == nullptr)
			{
				toDelete.Add(i);
			}			
		}
		
		for (int i = toDelete.Num() - 1; i >= 0; i++)
		{
			ArrayHelper.RemoveValues(toDelete[i], 1);
		}
	}	
}

void UArrayFunctions::Array_Compact(const TArray<int32>& TargetArray)
{
	// We should never hit these!  They're stubs to avoid NoExport on the class.  Call the Generic* equivalent instead
	check(0);
}
