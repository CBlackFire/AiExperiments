// <copyright file="SteamCallbackManager.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

/// <summary>
/// Class responsible for handling steam callbacks
/// </summary>
class SteamCallbackManager
{
public:
	//TODO:RemoveMiniCore

	/// <summary>
	/// Join requested callback
	/// </summary>
	//STEAM_CALLBACK(SteamCallbackManager, OnJoinRequested, GameRichPresenceJoinRequested_t, joinRequestedCallback);

	/// <summary>
	/// Micro transaction authorization response callback
	/// </summary>
	//STEAM_CALLBACK(SteamCallbackManager, OnMicroTxnAuthorizationResponse, MicroTxnAuthorizationResponse_t, microTxnAuthorizationResponseCallback);

	/// <summary>
	/// The game instance manager that created this instance
	/// </summary>
	class UClientGameInstanceManager* parent;

	/// <summary>
	/// Steam callback manager constructor.
	/// </summary>
	/// <returns></returns>
	SteamCallbackManager();
};