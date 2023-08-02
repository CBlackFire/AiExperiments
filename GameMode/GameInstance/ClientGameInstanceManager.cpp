// <copyright file="ClientGameInstanceManager.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>


#include "SoulShards.h"
#include "Player/Controller/MainMenuPlayerController.h"
#include "GameMode/GameInstance/SoulShardsGameInstance.h"
#include "PlayFabCpp/Public/Core/PlayFabMultiplayerDataModels.h"
#include "PlayFabCpp/Public/Core/PlayFabMultiplayerAPI.h"
#include <PlayFabCommon\Public\PlayFabCommon.h>
#include "SoulShardsHelper.hpp"
#include "ClientGameInstanceManager.h"

#include "../../Player/Controller/PlayerHttpCallManager.h"


#if UE_SERVER
#else
SteamCallbackManager UClientGameInstanceManager::steamCallbackManager;
#endif

UClientGameInstanceManager::UClientGameInstanceManager() 	
{
	this->UseLANForNetworking = false;
	this->LANPort = 7777;
	this->MultiplayerQueueWidget = nullptr;
	this->isQueuedForMultiPlayer = false;
	this->isMultiPlayerQueueTickSetup = false;
	this->multiPlayerQueueState = EQueuedMatchState::QMSLookingForPlayers;

	this->CurrentWidget = nullptr;
	this->PreviousWidget = nullptr;
	this->CurrentDialogWidget = nullptr;

	FString commandLine = FCommandLine::Get();

	if (!this->CEFDebugPort.IsEmpty())
	{
		bool cefDebugEnabled = FParse::Param(FCommandLine::Get(), TEXT("cefdebug"));
		if (!cefDebugEnabled)
		{
			commandLine += " -cefdebug=" + this->CEFDebugPort;
			FCommandLine::Set(*commandLine);
		}
	}

	this->state = EClientGameInstanceManagerState::ECIIdle;

	this->multipalyerAPI = nullptr;
}

//TODO:RemoveMiniCore
/*void UClientGameInstanceManager::OnJoinRequested(GameRichPresenceJoinRequested_t *pCallback)
{
	if (SoulShardsHelper::IsSteamUser())
	{
	
		// payload format:  /gameType:{0} /gameId:{1}
		FString payload(pCallback->m_rgchConnect);
		int gameTypeStart = payload.Find(TEXT("-gameType:"));
		int gameIdStart = payload.Find(TEXT("-gameId:"));

		if (gameTypeStart >= 0 && gameIdStart >= 0)
		{
			gameIdStart += 8;
			FString gameId = payload.Mid(gameIdStart, 32).TrimTrailing();
			FGuid::Parse(gameId, this->acceptedFriendChallengeId);	
		}
	}
}*/

//TODO:RemoveMiniCore
/*void UClientGameInstanceManager::OnMicroTxnAuthorizationResponse(MicroTxnAuthorizationResponse_t *pCallback)
{
	ISteamFriends* steamFriends = SteamFriends();
	if (SoulShardsHelper::IsSteamUser())
	{
		//This callback result contains the AppID, OrderID, and authorization state of the transaction.
		//uint32 m_unAppID;			// AppID for this microtransaction
		//uint64 m_ulOrderID;			// OrderID provided for the microtransaction
		//uint8 m_bAuthorized;		// if user authorized transaction

		if (pCallback->m_bAuthorized != 0)
		{
			AHttpEnabledPlayerController* playerController = static_cast<AHttpEnabledPlayerController*>(this->GetFirstLocalPlayerController());

			FHttpFailureEvent failureEvent;
			failureEvent.BindDynamic(playerController, &AHttpEnabledPlayerController::OnGameClientHttpFailure);

			playerController->GetHttpCallManager()->HttpConfirmPurchase(FString::FromInt(pCallback->m_ulOrderID));
		}		
	}
}*/

ULocalPlayer* UClientGameInstanceManager::GetFirstGamePlayer()
{
	USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(this->GetParentGameInstance());
	return gameInstance->GetFirstGamePlayer();
}

APlayerController* UClientGameInstanceManager::GetFirstLocalPlayerController()
{
	USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(this->GetParentGameInstance());
	return gameInstance->GetFirstLocalPlayerController();
}

UUserWidget* UClientGameInstanceManager::GetCurrentWidget()
{
	return this->CurrentWidget;
}

void UClientGameInstanceManager::SetCurrentWidget(UUserWidget* widget)
{
	this->PreviousWidget = this->CurrentWidget;
	this->CurrentWidget = widget;
}

UUserConfirmationDialog* UClientGameInstanceManager::GetCurrentDialogWidget()
{
	return this->CurrentDialogWidget;
}

void UClientGameInstanceManager::SetCurrentDialogWidget(UUserConfirmationDialog* dialogWidget)
{
	this->CurrentDialogWidget = dialogWidget;
}

void UClientGameInstanceManager::ClearWidgets()
{
	if (this->CurrentDialogWidget != nullptr)
	{
		this->CurrentDialogWidget->RemoveFromParent();
		this->CurrentDialogWidget = nullptr;
	}

	if (this->CurrentWidget != nullptr)
	{
		this->CurrentWidget->RemoveFromParent();
		this->CurrentWidget = nullptr;
	}
}


void UClientGameInstanceManager::JoinMultiplayerQueue(FClientGameData gameData)
{
	UE_LOG(SoulShardsLog, Log, TEXT("UClientGameInstanceManager::JoinMultiplayerQueue(): Start."));

	this->matchMakingTicket.Empty();
	// Set the queued game data
	this->queuedGameData = gameData;
	 
	if (!this->UseLANForNetworking)
	{

		this->SetupMultiplayerQueueTick();

		PlayFab::FPlayFabErrorDelegate onErrorDelegate = PlayFab::FPlayFabErrorDelegate::CreateLambda(
			[&](const PlayFab::FPlayFabCppError & errorResult)
			{
				UE_LOG(SoulShardsLog, Log, TEXT("UClientGameInstanceManager::JoinMultiplayerQueue(): CreateMatchmakingTicket Error, ErrorMessage: %s"), *errorResult.ErrorMessage);
				this->DropFromMultiplayerQueue(EQueuedMatchState::QMSFailedJoining);
			});

		PlayFab::UPlayFabMultiplayerAPI::FCreateMatchmakingTicketDelegate onSuccessDelegate =
			PlayFab::UPlayFabMultiplayerAPI::FCreateMatchmakingTicketDelegate::CreateLambda(
				[&, gameData](const PlayFab::MultiplayerModels::FCreateMatchmakingTicketResult & result)
				{
					UE_LOG(SoulShardsLog, Log, TEXT("UClientGameInstanceManager::JoinMultiplayerQueue(): CreateMatchmakingTicket Success, TickeID: %s"), *result.TicketId);

					this->matchMakingTicket = result.TicketId;

					// UClientGameInstanceManager queing state setup
					this->isQueuedForMultiPlayer = true;
					this->multiPlayerQueueState = EQueuedMatchState::QMSLookingForPlayers;

					this->queuedGameData.GameOptions = gameData.GameOptions;
					this->queuedGameData.GameOptions.GameType = gameData.GameOptions.GameType;
					this->queuedGameData.IsGameCreator = false;

					// Makes sure queue widget is displayed
					this->EnsureQueuedWidgetDisplayed();
				});

		// Setup playfab ticket request
		PlayFab::MultiplayerModels::FCreateMatchmakingTicketRequest ticketRequest;
		ticketRequest.Creator.Entity.Type = IPlayFabCommonModuleInterface::Get().GetEntityKeyType();
		ticketRequest.Creator.Entity.Id = IPlayFabCommonModuleInterface::Get().GetEntityKeyId();
		ticketRequest.QueueName = this->GetQueueNameForGameType(this->queuedGameData.GameOptions.GameType);
		//ticketRequest.GiveUpAfterSeconds = 86400;
		ticketRequest.GiveUpAfterSeconds = 100;

		// Set player attributes (Mock latencies for the time being).
		TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
		FString playerAttributesString =
			FString("{ ") +
			FString("\"DataObject\": { ") +
			FString("\"mu\": 16.0, ") +
			FString("\"sigma\" : 1.8, ") +
			FString("\"Latencies\" : [ ") +
			FString(" { ") +
			FString("\"region\": \"EastUs\", ") +
			FString("\"latency\" : 150 ") +
			FString("}, ") +
			FString("{ ") +
			FString("\"region\": \"WestUs\", ") +
			FString("\"latency\" : 160 ") +
			FString("} ") +
			FString("] ") +
			FString("} ") +
			FString("} ");

		TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(playerAttributesString);
		FJsonSerializer::Deserialize(jsonReader, jsonObject);
		ticketRequest.Creator.Attributes = MakeShareable(new PlayFab::MultiplayerModels::FMatchmakingPlayerAttributes(jsonObject));
		this->multipalyerAPI->CreateMatchmakingTicket(ticketRequest, onSuccessDelegate, onErrorDelegate);
	}
	else
	{
		this->ServerTravel(this->queuedGameData, "127.0.0.1", this->LANPort);
	}
}

void UClientGameInstanceManager::SetupMultiplayerQueueTick()
{
	// Setup multiplayer tick to check for ticket state
	if (!this->isMultiPlayerQueueTickSetup)
	{
		UWorld* world = this->GetParentGameInstance()->GetWorld();
		if (world)
		{
			FTimerHandle UnusedHandle;
			// Removes the notification after its time has passed
			world->GetTimerManager().SetTimer(
				UnusedHandle,
				this,
				&UClientGameInstanceManager::MultiplayerQueueTick,
				this->MultiPlayerQueueTickTimeSpan,
				true);

			UE_LOG(SoulShardsLog, Log, TEXT("UClientGameInstanceManager::SetParentGameInstance(): tick initialized."));
		}

		// This fetches the multiplayerAPI object from the PlayFab plugin, so you can make API calls with it.
		this->multipalyerAPI = IPlayFabModuleInterface::Get().GetMultiplayerAPI();

		this->isMultiPlayerQueueTickSetup = true;
	}
}

bool UClientGameInstanceManager::GetIsQueuedForMultiPlayer()
{
	return this->isQueuedForMultiPlayer;
}

void UClientGameInstanceManager::EnsureQueuedWidgetDisplayed()
{
	if (this->isQueuedForMultiPlayer)
	{
		if (this->MultiplayerQueueWidget == nullptr)
		{
			this->MultiplayerQueueWidget = CreateWidget<UUserWidget>(this->GetWorld(), this->GetMultiplayerQueueWidgetClass());
		}

		if (!this->MultiplayerQueueWidget->IsInViewport())
		{
			this->MultiplayerQueueWidget->AddToViewport(1000);
		}
	}
}

void UClientGameInstanceManager::MultiplayerQueueTick()
{
	if (this->isQueuedForMultiPlayer)
	{
		UE_LOG(SoulShardsLog, Log, TEXT("UClientGameInstanceManager::MultiplayerQueueTick(): Looking for multiplayer matches."));

		if (this->multiPlayerQueueState == EQueuedMatchState::QMSLookingForPlayers)
		{
			PlayFab::FPlayFabErrorDelegate onErrorDelegate = PlayFab::FPlayFabErrorDelegate::CreateLambda(
				[&](const PlayFab::FPlayFabCppError & errorResult)
				{
					// If the server is throttling us, just ignore the error and keep ticking
					if (errorResult.ErrorCode != 2054)
					{
						UE_LOG(SoulShardsLog, Log, TEXT("UClientGameInstanceManager::MultiplayerQueueTick(): GetMatchmakingTicket Error, ErrorMessage: %s"), *errorResult.ErrorMessage);
						this->DropFromMultiplayerQueue(EQueuedMatchState::QMSFailedJoining);
					}
				});

			PlayFab::UPlayFabMultiplayerAPI::FGetMatchmakingTicketDelegate onSuccessDelegate =
				PlayFab::UPlayFabMultiplayerAPI::FGetMatchmakingTicketDelegate::CreateLambda(
					[&](const PlayFab::MultiplayerModels::FGetMatchmakingTicketResult & result)
					{
						UE_LOG(SoulShardsLog, Log, TEXT("UClientGameInstanceManager::MultiplayerQueueTick(): GetMatchmakingTicket Success, MatchId: %s"), *result.Status);
						if (result.Status == FString("Matched"))
						{							
							this->StartMutiplayerMatch(result.MatchId);
						}
						else if (result.Status == FString("Canceled"))
						{
							this->DropFromMultiplayerQueue(EQueuedMatchState::QMSFailedJoining);
						}
					});

			PlayFab::MultiplayerModels::FGetMatchmakingTicketRequest ticketRequest;
			ticketRequest.QueueName = this->GetQueueNameForGameType(this->queuedGameData.GameOptions.GameType);
			ticketRequest.TicketId = this->matchMakingTicket;
			this->multipalyerAPI->GetMatchmakingTicket(ticketRequest, onSuccessDelegate, onErrorDelegate);

		}
	}
}

void UClientGameInstanceManager::StartMutiplayerMatch(FString matchId)
{
	float time = this->GetWorld()->GetTimeSeconds();

	this->multiPlayerQueueState = EQueuedMatchState::QMSStarting;
	PlayFab::FPlayFabErrorDelegate onErrorDelegate = PlayFab::FPlayFabErrorDelegate::CreateLambda(
		[&](const PlayFab::FPlayFabCppError & errorResult)
		{
			UE_LOG(SoulShardsLog, Log, TEXT("UClientGameInstanceManager::StartMutiplayerMatch(): GetMatch Error, ErrorMessage: %s"), *errorResult.ErrorMessage);

			this->DropFromMultiplayerQueue(EQueuedMatchState::QMSFailedJoining);
		});

	PlayFab::UPlayFabMultiplayerAPI::FGetMatchDelegate onSuccessDelegate =
		PlayFab::UPlayFabMultiplayerAPI::FGetMatchDelegate::CreateLambda(
			[&](const PlayFab::MultiplayerModels::FGetMatchResult & result)
			{
				UE_LOG(SoulShardsLog, Log, TEXT("UClientGameInstanceManager::StartMutiplayerMatch(): GetMatch Success, MatchId: %s"), *result.MatchId);				
				if (result.pfServerDetails.IsValid())
				{
					if (this->ServerTravel(this->queuedGameData, result.pfServerDetails->IPV4Address, result.pfServerDetails->Ports[0].Num))
					{
						this->DropFromMultiplayerQueue(EQueuedMatchState::QMSJoined);						
						return;
					}
				}

				this->DropFromMultiplayerQueue(EQueuedMatchState::QMSFailedJoining);				
			});

	PlayFab::MultiplayerModels::FGetMatchRequest matchRequest;
	matchRequest.QueueName = this->GetQueueNameForGameType(this->queuedGameData.GameOptions.GameType);
	matchRequest.MatchId = matchId;
	this->multipalyerAPI->GetMatch(matchRequest, onSuccessDelegate, onErrorDelegate);
}

bool UClientGameInstanceManager::ServerTravel(FClientGameData gameData, FString serverAddress, int serverPort)
{
	FString travelURL = this->GetTravelURL(serverAddress, serverPort);
	if (!travelURL.IsEmpty())
	{
		// If we are queued for multiplayer we restore the data we backed up when we entered the queue
		this->CurrentGameData = gameData;

		this->SetCurrentGameInstanceManager();

		FString lobbyOptions = this->GetLobbyOptionsForGameType(this->CurrentGameData.GameOptions);

		FString travelUrlWithGameType = travelURL + lobbyOptions;
		// Finally call the ClienTravel. 
		AHttpEnabledPlayerController* const playerController = static_cast<AHttpEnabledPlayerController*>(
			this->GetFirstLocalPlayerController());
		playerController->ClientTravel(travelUrlWithGameType, ETravelType::TRAVEL_Absolute);
		//playerController->ClientTravel("127.0.0.1:7777", ETravelType::TRAVEL_Absolute);// , ETravelType::TRAVEL_Absolute);

		return true;
	}

	return false;
}


FString UClientGameInstanceManager::GetTravelURL(FString serverAddress, int serverPort)
{
	FString travelURL;

	FString AuthToken;
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get(FName("PlayFab"));
	if (OnlineSubsystem != nullptr)
	{
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		if (Identity.IsValid())
		{
			AuthToken = Identity->GetAuthToken(0);
		}
	}

	travelURL = FString::Printf(TEXT("%s:%d"), *serverAddress, serverPort);

	// These URL tokens will have to be authenticated in the GameMode...
	// Why doesn't the Session get the info back to authenticate!?
	if (!AuthToken.IsEmpty())
	{
		travelURL = FString::Printf(TEXT("%s?AuthToken=%s"), *travelURL, *AuthToken);
	}

	return travelURL;
}



FClientGameData UClientGameInstanceManager::GetCurrentGameData()
{
	return this->CurrentGameData;
}

void UClientGameInstanceManager::SetCurrentGameData(FClientGameData gameData)
{
	this->CurrentGameData = gameData;
}

void UClientGameInstanceManager::SetParentGameInstance(UGameInstance* gameInstance)
{
	Super::SetParentGameInstance(gameInstance);

#if UE_SERVER
#else
	this->steamCallbackManager.parent = this;
#endif
}

void UClientGameInstanceManager::Tick()
{
	FGuid emptyGuid;

	// Start the game with friend if it's been accepted
	if (this->acceptedFriendChallengeId != emptyGuid)
	{
		AHttpEnabledPlayerController* playerController = static_cast<AHttpEnabledPlayerController*>(this->GetFirstLocalPlayerController());
		playerController->SelectDeckAndJoinFriendChallenge(this->acceptedFriendChallengeId);
		this->acceptedFriendChallengeId = emptyGuid;
	}
}

void UClientGameInstanceManager::DropFromMultiplayerQueue(EQueuedMatchState dropReason)
{
	UE_LOG(SoulShardsLog, Log, TEXT("AMainMenuPlayerController::DropFromMultiplayerQueue(): Start.  DropReaseon: %i"), (int)dropReason);

	// Set Client Manager state
	this->isQueuedForMultiPlayer = false;
	this->multiPlayerQueueState = dropReason;

	// Remove queue widget
	if (this->MultiplayerQueueWidget != nullptr)
	{
		if (this->MultiplayerQueueWidget->IsInViewport())
		{
			this->MultiplayerQueueWidget->RemoveFromViewport();
		}

		this->MultiplayerQueueWidget = nullptr;
	}

	if (dropReason != EQueuedMatchState::QMSJoined)
	{
		PlayFab::FPlayFabErrorDelegate onCancelErrorDelegate;
		PlayFab::UPlayFabMultiplayerAPI::FCancelMatchmakingTicketDelegate onCancelSuccessDelegate;

		PlayFab::MultiplayerModels::FCancelMatchmakingTicketRequest ticketRequest;
		ticketRequest.QueueName = this->GetQueueNameForGameType(this->queuedGameData.GameOptions.GameType);
		ticketRequest.TicketId = this->matchMakingTicket;
		// Try to drop current ticket (if there is one)
		this->multipalyerAPI->CancelMatchmakingTicket(ticketRequest, onCancelSuccessDelegate, onCancelErrorDelegate);

		APlayerController* playerController = this->GetFirstLocalPlayerController();

		// If the player manually dropped we don't retry.
		if (playerController && playerController->GetClass()->IsChildOf(AHttpEnabledPlayerController::StaticClass()) && dropReason != EQueuedMatchState::QMSDropped)
		{
			AHttpEnabledPlayerController* typedPlayerController = static_cast<AHttpEnabledPlayerController*>(playerController);
			// We failed finding a session so we notifiy the player controller so it might attempt a retry
			typedPlayerController->OnFailedJoinGame();
		}
	}

	this->matchMakingTicket.Empty();
}

EQueuedMatchState UClientGameInstanceManager::GetMultiPlayerQueueState()
{
	return this->multiPlayerQueueState;
}

void UClientGameInstanceManager::ManageLeaderboardResult(TArray<PlayFab::ClientModels::FPlayerLeaderboardEntry> leaderboardResult, const FGetLeaderboardEvent& onGetLeaderboardCompletedCallback)
{
	// Gets the subsystem player id.
	FString playFabId = this->GetOnlineSystemUserId();

	TArray<FLeaderboardEntryData> leaderboardEntryArray;
	for (int i = 0; i < leaderboardResult.Num(); i++)
	{
		PlayFab::ClientModels::FPlayerLeaderboardEntry entry = leaderboardResult[i];

		if ((entry.Position > 0) || (entry.StatValue > 0))
		{
			FLeaderboardEntryData leaderboardEntryData;
			if (!entry.DisplayName.IsEmpty())
			{
				leaderboardEntryData.FullDisplayName = entry.DisplayName;
				int fullDisplayNameSize = entry.DisplayName.Len();
				leaderboardEntryData.DisplayName = SoulShardsBaseHelper::ProfanityFilter(entry.DisplayName.Left(fullDisplayNameSize - 4));

				
			}

			USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(this->GetParentGameInstance());

			leaderboardEntryData.IsCurrentPlayer = playFabId == entry.PlayFabId;
			leaderboardEntryData.Position = entry.Position + 1;
			leaderboardEntryData.StatValue = entry.StatValue;
			leaderboardEntryArray.Add(leaderboardEntryData);
		}
		else
		{
			// Note: When calling 'GetLeaderboardAround...' APIs, the position of the user defaults to 0 when the user does not have the corresponding statistic.
			break;
		}
	}

	onGetLeaderboardCompletedCallback.ExecuteIfBound(leaderboardEntryArray);
}

void UClientGameInstanceManager::GetLeaderboard(FString statisticName, const FGetLeaderboardEvent& onGetLeaderboardCompletedCallback, FHttpFailureEvent onHttpCallFailed, int32 maxResultsCount, bool retry)
{
	// This fetches the clientAPI object from the PlayFab plugin, so you can make API calls with it.
	PlayFabClientPtr clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
	if (clientAPI->IsClientLoggedIn())
	{
		PlayFab::ClientModels::FGetLeaderboardRequest request;
		request.MaxResultsCount = maxResultsCount;
		request.StatisticName = statisticName;

		TSharedPtr<PlayFab::ClientModels::FPlayerProfileViewConstraints> profileConstraints(new PlayFab::ClientModels::FPlayerProfileViewConstraints());
		profileConstraints->ShowDisplayName = true;
		request.ProfileConstraints = profileConstraints;

		clientAPI->GetLeaderboard(
			request,
			PlayFab::UPlayFabClientAPI::FGetLeaderboardDelegate::CreateLambda(
				[&, onGetLeaderboardCompletedCallback](const PlayFab::ClientModels::FGetLeaderboardResult& result)
		{
			this->ManageLeaderboardResult(result.Leaderboard, onGetLeaderboardCompletedCallback);
		}),
			PlayFab::FPlayFabErrorDelegate::CreateLambda(
				[&, statisticName, onGetLeaderboardCompletedCallback, onHttpCallFailed, maxResultsCount](const PlayFab::FPlayFabCppError & ErrorResult)
		{
			if (retry)
			{
				PlayFab::ClientModels::FLoginWithSteamRequest request = AHttpEnabledPlayerController::CreateLoginWithSteamRequest();
				clientAPI->LoginWithSteam(
					request,
					PlayFab::UPlayFabClientAPI::FLoginWithSteamDelegate::CreateLambda(
						[&, statisticName, onGetLeaderboardCompletedCallback, onHttpCallFailed, maxResultsCount](const PlayFab::ClientModels::FLoginResult& result)
				{
					this->GetLeaderboard(statisticName, onGetLeaderboardCompletedCallback, onHttpCallFailed, maxResultsCount, false /*retry*/);
				}),
					PlayFab::FPlayFabErrorDelegate::CreateLambda(
						[&, onHttpCallFailed](const PlayFab::FPlayFabCppError & ErrorResult)
				{
					onHttpCallFailed.ExecuteIfBound(EHttpFailureReason::HFRServerError);
				}));
			}
			else
			{
				onHttpCallFailed.ExecuteIfBound(EHttpFailureReason::HFRServerError);
			}
		}));
	}
}

void UClientGameInstanceManager::GetLeaderboardAroundPlayer(FString statisticName, const FGetLeaderboardEvent& onGetLeaderboardCompletedCallback, FHttpFailureEvent onHttpCallFailed, int32 maxResultsCount, bool retry)
{
	// This fetches the clientAPI object from the PlayFab plugin, so you can make API calls with it.
	PlayFabClientPtr clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	if (clientAPI->IsClientLoggedIn())
	{
		PlayFab::ClientModels::FGetLeaderboardAroundPlayerRequest request;
		request.MaxResultsCount = maxResultsCount;
		request.StatisticName = statisticName;

		TSharedPtr<PlayFab::ClientModels::FPlayerProfileViewConstraints> profileConstraints(new PlayFab::ClientModels::FPlayerProfileViewConstraints());
		profileConstraints->ShowDisplayName = true;
		//request.ProfileConstraints = profileConstraints;

		clientAPI->GetLeaderboardAroundPlayer(
			request,
			PlayFab::UPlayFabClientAPI::FGetLeaderboardAroundPlayerDelegate::CreateLambda(
				[&, onGetLeaderboardCompletedCallback](const PlayFab::ClientModels::FGetLeaderboardAroundPlayerResult& result)
		{
			this->ManageLeaderboardResult(result.Leaderboard, onGetLeaderboardCompletedCallback);
		}),
			PlayFab::FPlayFabErrorDelegate::CreateLambda(
				[&, statisticName, onGetLeaderboardCompletedCallback, onHttpCallFailed, maxResultsCount](const PlayFab::FPlayFabCppError & ErrorResult)
		{
			if (retry)
			{
				PlayFab::ClientModels::FLoginWithSteamRequest request = AHttpEnabledPlayerController::CreateLoginWithSteamRequest();
				clientAPI->LoginWithSteam(
					request,
					PlayFab::UPlayFabClientAPI::FLoginWithSteamDelegate::CreateLambda(
						[&, statisticName, onGetLeaderboardCompletedCallback, onHttpCallFailed, maxResultsCount](const PlayFab::ClientModels::FLoginResult& result)
				{
					this->GetLeaderboardAroundPlayer(statisticName, onGetLeaderboardCompletedCallback, onHttpCallFailed, maxResultsCount, false /*retry*/);
				}),
					PlayFab::FPlayFabErrorDelegate::CreateLambda(
						[&, onHttpCallFailed](const PlayFab::FPlayFabCppError & ErrorResult)
				{
					onHttpCallFailed.ExecuteIfBound(EHttpFailureReason::HFRServerError);
				}));
			}
			else
			{
				onHttpCallFailed.ExecuteIfBound(EHttpFailureReason::HFRServerError);
			}
		}));
	}
}

FString UClientGameInstanceManager::GetOnlineSystemUserId()
{
	const int32 controllerId = CastChecked<ULocalPlayer>(this->GetFirstLocalPlayerController()->Player)->GetControllerId();

	const FName defaultOnlineSubsystemName = UOnlineEngineInterface::Get()->GetDefaultOnlineSubsystemName();
	IOnlineSubsystem* platformOnlineSub = IOnlineSubsystem::Get(defaultOnlineSubsystemName);
	if (!platformOnlineSub)
	{
		return FString();
	}

	IOnlineIdentityPtr onlineIdentity = platformOnlineSub->GetIdentityInterface();
	if (!onlineIdentity)
	{
		return FString();
	}

	TSharedPtr<const FUniqueNetId> owningUserId = onlineIdentity->GetUniquePlayerId(controllerId);
	return owningUserId->ToString();
}

bool UClientGameInstanceManager::GertUseLANForNetworking()
{
	return this->UseLANForNetworking;
}

TSubclassOf<UUserWidget> UClientGameInstanceManager::GetMultiplayerQueueWidgetClass()
{
	if (this->MultiplayerQueueWidgetClass.IsNull())
	{
		this->MultiplayerQueueWidgetClass = SoulShardsHelper::GetClassByString("/Game/UI/UI_MultiPlayerQueueWidget.UI_MultiPlayerQueueWidget_C", this);
	}

	return this->MultiplayerQueueWidgetClass.LoadSynchronous();
}