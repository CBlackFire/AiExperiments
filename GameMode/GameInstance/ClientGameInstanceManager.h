	// <copyright file="ClientGameInstanceManager.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/GameInstance/BaseGameInstanceManager.h"
#include "GameMode/GameInstance/SteamCallbackManager.h"
#include "Player/Controller/HttpEnabledPlayerController.h"
#include "GameMode/Data/ClientGameData.h"
#include "PlayFab.h"
#include "ClientGameInstanceManager.generated.h"

UCLASS(config = Game, BlueprintType)
/// <summary>
/// Class responsible for handling specific aspects of client game instances.
/// </summary>
/// <seealso cref="UBaseGameInstanceManager" />
/// <seealso cref="UObject" />
class SOULSHARDS_API UClientGameInstanceManager : public UBaseGameInstanceManager
{
	GENERATED_BODY()
	
private:	

	UPROPERTY(Config)
	/// <summary>
	/// Determines whether to use LAN for networking (for debugging scenarios)
	/// </summary>
	bool UseLANForNetworking;

	UPROPERTY(Config)
	/// <summary>
	/// Determines LAN port to connect to
	/// </summary>
	int LANPort;

	UPROPERTY(Config)
	/// <summary>
	/// Chromium embedded framework debug port.
	/// </summary>
	FString CEFDebugPort;

	UPROPERTY(Config)
	/// <summary>
	/// Time in seconds before the the next multiplayer tick is sent
	/// </summary>
	float MultiPlayerQueueTickTimeSpan;

	UPROPERTY()
	/// <summary>
	/// The multiplayer queue widget class.  Used to create the multi player queue widget when needed.
	/// </summary>
	TSoftClassPtr<UUserWidget> MultiplayerQueueWidgetClass;

	UPROPERTY()
	/// <summary>
	/// The current widget.  This exists here in the game instance so we can keep track of it across levels 
	/// </summary>
	UUserWidget* MultiplayerQueueWidget;

	UPROPERTY()
	/// <summary>
	/// The previous widget.  In some scenarios we have widgets that navigate to another widget (i.e. loading screen) but still have some event handlers while
	/// the new screen is active.  In order to avoid the previous screen to be garbage collected we keep a reference to it.
	/// </summary>
	UUserWidget* PreviousWidget;

	UPROPERTY()
	/// <summary>
	/// The current widget.  This exists here in the game instance so we can keep track of it across levels 
	/// </summary>
	UUserWidget* CurrentWidget;

	UPROPERTY()
	/// <summary>
	/// The current widget.  This exists here in the game instance so we can keep track of it across levels 
	/// </summary>
	class UUserConfirmationDialog* CurrentDialogWidget;

	/// <summary>
	/// The accepted friend challenge identifier.  This will be used to initiate the joining of the friend challenge during the tick function.
	/// </summary>
	FGuid acceptedFriendChallengeId;

	/// <summary>
	/// Flag that determines whether the player is queued for multi player
	/// </summary>
	bool isQueuedForMultiPlayer;

	/// <summary>
	/// The multi player queue state
	/// </summary>
	EQueuedMatchState multiPlayerQueueState;

	/// <summary>
	/// A flat indicating whether the multi player queue tick is already setup
	/// </summary>
	bool isMultiPlayerQueueTickSetup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaCharacter, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// The options that the user selected for the current game
	/// </summary>
	FClientGameData CurrentGameData;

	/// <summary>
	/// The options that user selected when queueing a multiplayer game
	/// </summary>
	FClientGameData queuedGameData;

	/// <summary>
	/// Client Game Instace Manager state
	/// </summary>
	EClientGameInstanceManagerState state;

	/// <summary>
	/// This is an object that lets you access the PlayFab client API
	/// </summary>
	PlayFabMultiplayerPtr multipalyerAPI = nullptr;

	/// <summary>
	/// Ticket used for matchmaking purposes
	/// </summary>
	FString matchMakingTicket;
	
#if UE_SERVER
#else
	/// <summary>
	/// The steam invite callback manager
	/// </summary>
	static SteamCallbackManager steamCallbackManager;
#endif
	/// <summary>
	/// Gets the travel path for current game type.  Thisi s the travel path to the dedicated server lobby
	/// </summary>
	/// <returns></returns>
	FString GetClientTravelPath();

	/// <summary>
	/// Gets the first game player.
	/// </summary>
	/// <returns></returns>
	ULocalPlayer* GetFirstGamePlayer();

	/// <summary>
	/// Gets the first local player controller.
	/// </summary>
	/// <returns></returns>
	APlayerController* GetFirstLocalPlayerController();

	/// <summary>
	/// Encapsulate the management of the leaderboard result.
	/// </summary>
	/// <param name="leaderboardResult">The leaderboard result.</param>
	/// <param name="onGetLeaderboardCompletedCallback">The callback that gets invoked when the leaderboard entry list is retrieved.</param>
	void ManageLeaderboardResult(TArray<PlayFab::ClientModels::FPlayerLeaderboardEntry> leaderboardResult, const FGetLeaderboardEvent& onGetLeaderboardCompletedCallback);

	/// <summary>
	/// Sets up the multiplayer queue tick
	/// </summary>
	void SetupMultiplayerQueueTick();

	/// <summary>
	/// Starts a match with a known ID
	/// </summary>
	void StartMutiplayerMatch(FString matchId);

	/// <summary>
	/// Gets the travel URL from serverAddress/serverPort
	/// </summary>
	FString GetTravelURL(FString serverAddress, int serverPort);

	/// <summary>
	/// Starts the server travel
	/// </summary>
	bool ServerTravel(FClientGameData gameData, FString serverAddress, int serverPort);
	
	/// <summary>
	/// Gets the multiplayer queue widget class.
	/// </summary>
	TSubclassOf<UUserWidget> GetMultiplayerQueueWidgetClass();
	
protected:

	/// <summary>
	/// Ticks the multiplayer queue
	/// </summary>
	virtual void MultiplayerQueueTick();

	/// <summary>
	/// Ticks this instance.  Custom tick implementation just for instance managers, based on timers
	/// </summary>
	virtual void Tick() override;

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="UClientGameInstanceManager"/> class.
	/// </summary>
	UClientGameInstanceManager();

#pragma region Client Methods

	/// <summary>
	/// Joins the multiplayer queue on the client side
	/// </summary>
	/// <param name="gameData">Game data.</param>
	void JoinMultiplayerQueue(FClientGameData gameData);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = UClientGameInstanceManager)
	/// <summary>
	/// Gets the is queued for multiplayer.
	/// </summary>
	/// <returns></returns>
	bool GetIsQueuedForMultiPlayer();

#pragma endregion

#pragma region Widgets
	/// <summary>
	/// Gets the current widget.
	/// </summary>
	/// <returns></returns>
	UUserWidget* GetCurrentWidget();

	/// <summary>
	/// Sets the current widget.
	/// </summary>
	/// <param name="widget">The widget.</param>
	void SetCurrentWidget(UUserWidget* widget);

	/// <summary>
	/// Gets the current dialog widget.
	/// </summary>
	/// <returns></returns>
	class UUserConfirmationDialog* GetCurrentDialogWidget();

	/// <summary>
	/// Gets the current dialog widget.
	/// </summary>
	/// <param name="dialogWidget">The dialog widget.</param>
	void SetCurrentDialogWidget(class UUserConfirmationDialog* dialogWidget);

	/// <summary>
	/// Clears the widgets and removes them from viewport.
	/// </summary>
	/// <param name="dialogWidget">The dialog widget.</param>
	void ClearWidgets();

	/// <summary>
	/// If the player is queued for multiplayer, ensure the widget is displayed
	/// </summary>
	void EnsureQueuedWidgetDisplayed();

#pragma endregion

	/// <summary>
	/// Gets the current game data
	/// </summary>
	/// <returns></returns>
	FClientGameData GetCurrentGameData();

	UFUNCTION(BlueprintCallable, Category = UClientGameInstanceManager)
	/// <summary>
	/// Sets the current game data.
	/// </summary>
	/// <returns></returns>
	void SetCurrentGameData(FClientGameData gameData);

	/// <summary>
	/// Called when a request to join a friend's game has been accepted
	/// </summary>
	/// <param name="pCallback">The p callback.</param>
	//void OnJoinRequested(GameRichPresenceJoinRequested_t *pCallback);
	//TODO:RemoveMiniCore

	/// <summary>
	/// Called when a user has responded to a microtransaction authorization request
	/// </summary>
	/// <param name="pCallback">The p callback.</param>
	//void OnMicroTxnAuthorizationResponse(MicroTxnAuthorizationResponse_t *pCallback);
	//TODO:RemoveMiniCore

	/// <summary>
	/// Sets the parent game instance
	/// </summary>
	virtual void SetParentGameInstance(UGameInstance* gameInstance) override;

	UFUNCTION(BlueprintCallable, Category = UClientGameInstanceManager)
	/// <summary>
	/// Drops from the multiplayer queue is the user is already queued.
	/// </summary>
	void DropFromMultiplayerQueue(EQueuedMatchState dropReason = EQueuedMatchState::QMSDropped);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = UClientGameInstanceManager)
	/// <summary>
	/// Gets the multiplayer queue state.
	/// </summary>
	/// <returns></returns>
	EQueuedMatchState GetMultiPlayerQueueState();

	UFUNCTION(BlueprintCallable, Category = AHttpEnabledPlayerController)
	/// <summary>
	/// Gets an array with leaderboards.
	/// </summary>
	/// <param name="statisticName">Unique identifier for the title-specific statistic for the leaderboard.</param>
	/// <param name="onGetLeaderboardCompletedCallback">The callback that gets invoked when the leaderboard entry list is retrieved.</param>
	/// <param name="onHttpCallFailed">The callback that gets invoked when get leaderboard fails.</param>
	/// <param name="maxResultsCount">Maximum number of entries to retrieve. Default 10, maximum 100.</param>
	/// <param name="retry">Retry in case of error.</param>
	void GetLeaderboard(FString statisticName, const FGetLeaderboardEvent& onGetLeaderboardCompletedCallback, FHttpFailureEvent onHttpCallFailed, int32 maxResultsCount = 10, bool retry = true);

	UFUNCTION(BlueprintCallable, Category = AHttpEnabledPlayerController)
	/// <summary>
	/// Gets an array with leaderboards.
	/// </summary>
	/// <param name="statisticName">Unique identifier for the title-specific statistic for the leaderboard.</param>
	/// <param name="onGetLeaderboardCompletedCallback">The callback that gets invoked when the leaderboard entry list is retrieved.</param>
	/// <param name="onHttpCallFailed">The callback that gets invoked when get leaderboard fails.</param>
	/// <param name="maxResultsCount">Maximum number of entries to retrieve. Default 10, maximum 100.</param>
	/// <param name="retry">Retry in case of error.</param>
	void GetLeaderboardAroundPlayer(FString statisticName, const FGetLeaderboardEvent& onGetLeaderboardCompletedCallback, FHttpFailureEvent onHttpCallFailed, int32 maxResultsCount = 10, bool retry = true);

	/// <summary>
	/// Gets the user id from the online system
	/// </summary>
	FString GetOnlineSystemUserId();

	/// <summary>
	/// Gets whether the game should use LAN for networking.  Used for debugging scenarios
	/// </summary>
	bool GertUseLANForNetworking();
};
