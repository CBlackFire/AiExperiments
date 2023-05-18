// <copyright file="ArrayFunctions.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
//
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once 

#include "ArrayFunctions.generated.h"

UCLASS()
/// <summary>
/// Class containing array helper functions.
/// </summary>
class SOULSHARDSBASE_API UArrayFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

protected:

public:

	/// <summary>
	/// Removes null elements from an array
	/// </summary>
	UFUNCTION(BlueprintCallable, CustomThunk, meta=(DisplayName = "Compact Array Elements", CompactNodeTitle = "COMPACT", ArrayParm = "TargetArray"), Category="Utilities|Array")
	static void Array_Compact(const TArray<int32>& TargetArray);

	// Native functions that will be called by the below custom thunk layers, which read off the property address, and call the appropriate native handler
	static void GenericArray_Compact(const void* TargetArray, const FArrayProperty* ArrayProp);

	DECLARE_FUNCTION(execArray_Compact)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(nullptr);
		void* ArrayAddr = Stack.MostRecentPropertyAddress;
		FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}

		P_GET_PROPERTY(FIntProperty, First);
		P_GET_PROPERTY(FIntProperty, Second);

		P_FINISH;
		P_NATIVE_BEGIN;
		MARK_PROPERTY_DIRTY(Stack.Object, ArrayProperty);
		GenericArray_Compact(ArrayAddr, ArrayProperty);
		P_NATIVE_END;
	}

	
};
