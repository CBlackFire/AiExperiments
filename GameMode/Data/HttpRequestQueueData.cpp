// <copyright file="HttpRequestQueueData.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "HttpRequestQueueData.h"

FHttpRequestQueueData::FHttpRequestQueueData()
{
	this->Url = NULL;
	this->Content = NULL;
	this->LastTryTime = 0.0f;
	this->TriesCount = 0;
	this->NeedRemove = false;
}

FHttpRequestQueueData::FHttpRequestQueueData(FString url, FString content)
{
	this->Url = url;
	this->Content = content;
	this->LastTryTime = 0.0f;
	this->TriesCount = 0;
	this->NeedRemove = false;
}

bool FHttpRequestQueueData::ValidateSend(float currentTime, float delayInSecondsBeforeSubmitMatchRetry, int maxSubmitMatchTries)
{
	if (!this->NeedRemove)
	{
		if ((this->LastTryTime == 0.0f) || ((currentTime - this->LastTryTime) > delayInSecondsBeforeSubmitMatchRetry))
		{
			this->LastTryTime = currentTime;
			this->TriesCount += 1;

			if (this->TriesCount <= maxSubmitMatchTries)
			{
				return true;
			}
			else
			{
				this->NeedRemove = true;
			}
		}
	}

	return false;
}