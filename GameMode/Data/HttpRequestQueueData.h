// <copyright file="HttpRequestQueueData.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "HttpRequestQueueData.generated.h"


USTRUCT(BlueprintType)
/// <summary>
/// Data for the http request queue
/// </summary>
struct SOULSHARDS_API FHttpRequestQueueData
{
	GENERATED_USTRUCT_BODY()
	
public:

	FHttpRequestQueueData();

	FHttpRequestQueueData(FString url, FString content);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FHttpRequestQueueData)
	/// <summary>
	/// The url
	/// </summary>
	FString Url;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FHttpRequestQueueData)
	/// <summary>
	/// The content of the request
	/// </summary>
	FString Content;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FHttpRequestQueueData)
	/// <summary>
	/// The last time when the request was sent.
	/// </summary>
	float LastTryTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FHttpRequestQueueData)
	/// <summary>
	/// The number of times this request was sent.
	/// </summary>
	int TriesCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FHttpRequestQueueData)
	/// <summary>
	/// The value indicating whether this request need to be removed.
	/// </summary>
	bool NeedRemove;

	/// <summary>
	/// Check if the request can be sent at this time and update its info and if it requires to be removed.
	/// </summary>
	bool ValidateSend(float currentTime, float delayInSecondsBeforeSubmitMatchRetry, int maxSubmitMatchTries);
};
