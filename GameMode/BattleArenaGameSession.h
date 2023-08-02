// <copyright file="BattleArenaGameSession.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameFramework/GameSession.h"
#include "BattleArenaGameSession.generated.h"

UCLASS()
/// <summary>
/// Game session for a battle arena game.
/// </summary>
/// <seealso cref="AGameSession" />
class SOULSHARDS_API ABattleArenaGameSession : public AGameSession
{
	GENERATED_BODY()

	/// <summary>
	/// Session settings
	/// </summary>
	TSharedPtr<class FOnlineSessionSettings> hostSettings;

	/// <summary>
	/// Determines whether the current game session is a dedicated server lobby
	/// </summary>
	/// <returns>
	///   <c>true</c> if this session is a dedicated server lobby; otherwise, <c>false</c>.
	/// </returns>
	bool IsDedicatedServerLobby();

public:

	/// <summary>
	/// Registers the server in case this is a dedicated server.
	/// </summary>
	virtual void RegisterServer() override;

	/// <summary>
	/// Approves the login.  Called from GameMode.PreLogin() and Login().
	/// </summary>
	/// <param name="options">Options	The URL options (e.g. name/spectator) the player has passed</param>
	/// <returns>Non-empty Error String if player not approved</returns>
	virtual FString ApproveLogin(const FString& options) override;

	/// <summary>
	/// Called by GameMode::PostLogin to give session code chance to do work after PostLogin
	/// </summary>
	/// <param name="NewPlayer">The new player.</param>
	virtual void PostLogin(APlayerController* newPlayer);

	/// <summary>
	/// Notifies the logout of a player
	/// </summary>
	/// <param name="controller">The player controller.</param>
	virtual void NotifyLogout(const APlayerController* controller);
};
