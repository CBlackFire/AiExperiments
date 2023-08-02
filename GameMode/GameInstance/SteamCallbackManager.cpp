// <copyright file="SteamCallbackManager.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>


#include "SoulShards.h"
#include "GameMode/GameInstance/ClientGameInstanceManager.h"
#include "SteamCallbackManager.h"

//TODO:RemoveMiniCore
SteamCallbackManager::SteamCallbackManager() /*:
	joinRequestedCallback(this, &SteamCallbackManager::OnJoinRequested),
	microTxnAuthorizationResponseCallback(this, &SteamCallbackManager::OnMicroTxnAuthorizationResponse)*/
{
	this->parent = nullptr;
}

//TODO:RemoveMiniCore
/*void SteamCallbackManager::OnJoinRequested(GameRichPresenceJoinRequested_t *pCallback)
{
	if (this->parent)
	{
		this->parent->OnJoinRequested(pCallback);
	}
}

void SteamCallbackManager::OnMicroTxnAuthorizationResponse(MicroTxnAuthorizationResponse_t *pCallback)
{
	if (this->parent)
	{
		//this->parent->OnMicroTxnAuthorizationResponse(pCallback);
	}
}*/
