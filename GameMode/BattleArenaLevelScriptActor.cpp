// <copyright file="BattleArenaLevelScriptActor.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#include "SoulShards.h"
#include "GameMode/BattleArenaGameMode.h"
#include "Character/SoulCaster/Actor/SoulShardsCharacter.h"
#include "Character/SoulCaster/Actor/TotemCharacter.h"
#include "GameMode/GameInstance/SoulShardsGameInstance.h"
#include "Player/Controller/BattleArenaPlayerController.h"
#include "Player/Controller/PlayerInputController.h"
#include "PlayFab.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "SoulShardsHelper.hpp"
#include "DynamicObjectPool.h"
#include "BattleArenaLevelScriptActor.h"

ABattleArenaLevelScriptActor::ABattleArenaLevelScriptActor()
{
	this->LevelState = ELevelState::ELSIntroPending;
	
	this->BrightnessLevel = 1.0;
	this->bNetLoadOnClient = true;
	
	this->SetActorTickEnabled(true);
	this->PrimaryActorTick.bCanEverTick = true;
	this->introLoopsPlayed = 0;
	this->bReplicates = false;
	this->PlayAutomatedIntroOnStart = true;
}

class ABattleArenaPlayerController* ABattleArenaLevelScriptActor::GetFirstBattleArenaPlayerController() const
{
	const bool isDedicatedServer = this->GetNetMode() == ENetMode::NM_DedicatedServer;
	ABattleArenaPlayerController* toReturn = nullptr;
	UWorld* world = this->GetWorld();
	if (isDedicatedServer)
	{
		toReturn = Cast<ABattleArenaPlayerController>(world->GetFirstPlayerController());
	} else
	{		
		toReturn = SoulShardsHelper::GetFirstLocalNonReplayPlayerController(world);
	}
	
	return toReturn;
}


void ABattleArenaLevelScriptActor::SetViewCameraToStart(UWorld* world) const
{		
	// We use bAutoManageActiveCameraTarget to figure out if intro is playing.  We need to do this beacuse post load might be called for maps that are not the current map (UE4 bug?)
	if (this->LevelState != ELevelState::ELSPlayingLevelIntro && this->LevelState != ELevelState::ELSPlayingPlayerIntro)
	{
		ABattleArenaPlayerController* battleArenaPlayerController = this->GetFirstBattleArenaPlayerController();
		APlayerController* playerController = battleArenaPlayerController;
		const bool isPlayingReplay = SoulShardsHelper::IsPlayingReplay(world);
		if (battleArenaPlayerController == nullptr && isPlayingReplay)
		{
			playerController = world->GetFirstPlayerController();
		}
		 
		if (playerController)
		{
			AActor* actualTarget = playerController->GetViewTarget();
			TArray<AActor*> cameras;
			UGameplayStatics::GetAllActorsWithTag(this->GetWorld(), "StartCamera", cameras);		
			if (cameras.Num() > 0)
			{
				AActor* nextTarget = cameras[0];
				if (nextTarget != actualTarget)
				{
					FViewTargetTransitionParams params;
					params.BlendTime = 0;
					playerController->SetViewTarget(cameras[0], params);

					// There are certain parameters that won't apply to replay camera.
					if (!isPlayingReplay)
					{
						playerController->bAutoManageActiveCameraTarget = false;
						playerController->PlayerCameraManager->bClientSimulatingViewTarget = true;
					}
				}
			}
		}
	}
}

void ABattleArenaLevelScriptActor::OnPostLoadMapWithWorld(UWorld* world) const
{
	if (this->LevelState == ELevelState::ELSIntroPending || this->LevelState == ELevelState::ELSNotServingMatch)
	{		
		this->SetViewCameraToStart(world);		
	}
}

bool ABattleArenaLevelScriptActor::TryPlayIntro()
{
	if (this->PlayAutomatedIntroOnStart && this->LevelState == ELevelState::ELSIntroPending)
	{
		UWorld* world = this->GetWorld();
		ABattleArenaPlayerController* controller = this->GetFirstBattleArenaPlayerController();
		const bool isPlayingReplay = SoulShardsHelper::IsPlayingReplay(world);

		// Replay doesn't play totem intro.
		if (isPlayingReplay)
		{
			this->LevelState = ELevelState::ELSIntroPlayed;
			return false;
		}

		if (controller)
		{
			
			ABattleArenaGameMode* gameMode = Cast<ABattleArenaGameMode>(world->GetAuthGameMode());

			controller->bAutoManageActiveCameraTarget = false;

			const bool isClient = (this->GetNetMode() == ENetMode::NM_Client);
			const bool isGameHost = gameMode != nullptr;

			if (isClient || isGameHost)
			{
				AReplayCamera* replayCamera = Cast<AReplayCamera>(this->GetReplayCameraActor());
				if (replayCamera)
				{
					this->LevelState = ELevelState::ELSPlayingLevelIntro;
					FViewTargetTransitionParams params;

					controller->PlayerCameraManager->bClientSimulatingViewTarget = true;
					
					replayCamera->PlayAutoamtedCameraScript(this->GetCameraScript("LevelIntro"), params);
				}
			}
			else
			{
				this->LevelState = ELevelState::ELSNotServingMatch;
			}			
			
			return true;
		}
	}

	return false;
}

void ABattleArenaLevelScriptActor::TryPlayAutomatedCameraScript()
{

	// If this is replay and json steps are loaded then plays the automated camera script with these steps
	if (SoulShardsHelper::IsPlayingReplay(this->GetWorld()))
	{
		ABattleArenaPlayerController* battleArenaPlayerController = this->GetFirstBattleArenaPlayerController();
		AReplayCamera* replayCamera = Cast<AReplayCamera>(this->GetReplayCameraActor());
		if (replayCamera && battleArenaPlayerController->GetViewTarget() != replayCamera) {
			FReplayCameraScriptStepsData replayCameraScriptStepsData = replayCamera->GetReplayCameraScriptStepsData();
			if (replayCameraScriptStepsData.ScriptSteps.Num() > 0)
			{
				FViewTargetTransitionParams params;
				replayCamera->PlayAutoamtedCameraScript(replayCameraScriptStepsData, params);
			}
		}
	}
}

void ABattleArenaLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = this->GetWorld();
	ABattleArenaGameMode* gameMode = Cast<ABattleArenaGameMode>(world->GetAuthGameMode());
	const bool isDedicatedServer = this->GetNetMode() == ENetMode::NM_DedicatedServer;
	ABattleArenaPlayerController* controller = this->GetFirstBattleArenaPlayerController();	

	// We need to make sure ReplayCamera actor is here so it can be used by replays or intro)
	if (gameMode != nullptr)
	{
		this->GetReplayCameraActor();
	}	

	if (this->PlayAutomatedIntroOnStart)
	{
		this->TryPlayIntro();
	}
	else
	{
		this->SetViewCameraToStart(this->GetWorld());
		
		if (this->LevelState == ELevelState::ELSIntroPending)
		{
			this->LevelState = ELevelState::ELSIntroPlayed;
		}
	}

	if (!isDedicatedServer)
	{
		if (controller)
		{
			controller->CreateHudWidget(false);
		}

		// In case this is the mp start level we do a version compatibility check
		const FString currentLevel = UGameplayStatics::GetCurrentLevelName(this->GetWorld());
		if (currentLevel.Contains(TEXT("start"), ESearchCase::IgnoreCase))
		{
			if (GetWorld()->WorldType != EWorldType::Editor)
			{
				this->CheckVersionCompatibility();
			}
		}
	}
}

void ABattleArenaLevelScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (this->ReplayCamera)
	{
		this->ReplayCamera->Destroy();
		this->ReplayCamera = nullptr;
	}
	Super::EndPlay(EndPlayReason);

	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

TArray<class UMaterialInterface*> ABattleArenaLevelScriptActor::GetMaterialInterfaces(UMaterialInterface* baseMaterialInterface)
 {
	 check(this->PostProcessVolume);
	 const FPostProcessVolumeProperties properties = this->PostProcessVolume->GetProperties();
	 FWeightedBlendables weightedBlendables = properties.Settings->WeightedBlendables;
	 TArray<UMaterialInterface*> toReturn;

	 for (int32 i = 0, count = weightedBlendables.Array.Num(); i < count; ++i)
	 {	 
		 UMaterialInterface* material = static_cast<UMaterialInterface*> (weightedBlendables.Array[i].Object);

	 	if (material == nullptr)
	 		continue;
		 
		 if (material == baseMaterialInterface)
		 {
			 toReturn.Add(material);
		 }
		 else if(material->GetClass()->IsChildOf(UMaterialInstance::StaticClass()))
		 {
			 UMaterialInstance* materialInstance = static_cast<UMaterialInstance*> (material);
			 if (materialInstance->Parent == baseMaterialInterface)
			 {
				 toReturn.Add(material);
			 }
		 }
	 }

	 return toReturn;
 }

void ABattleArenaLevelScriptActor::TriggerPhaseEvent(FString eventName, UObject* target)
{
	ABattleArenaGameMode* gameMode = Cast<ABattleArenaGameMode>(this->GetWorld()->GetAuthGameMode());
	if (gameMode)
	{
		gameMode->TriggerPhaseEvent(eventName, target);

		if (this->ReplayCamera && (eventName.Contains("OnCreatureSpawn") || eventName.Contains("OnCreatureDeath")))
		{
			this->ReplayCamera->OnArenaChangedEvent();
		}
	}
}

void ABattleArenaLevelScriptActor::SetLocalPlayerInputMask(int inputMask)
{
	for (TActorIterator<APlayerController> it(this->GetWorld()); it; ++it)
	{
		APlayerController* playerController = *it;
		ASoulShardsCharacter* caster = Cast<ASoulShardsCharacter>(playerController->GetPawn());
		if (caster)
		{
			UPlayerInputController* inputController = caster->GetInputController();
			if (inputController)
			{
				inputController->SetInputMask(inputMask);
			}
		}
	}
}

void ABattleArenaLevelScriptActor::SetLocalPlayerInputMask2(int inputMask)
{
	for (TActorIterator<APlayerController> it(this->GetWorld()); it; ++it)
	{
		APlayerController* playerController = *it;
		ASoulShardsCharacter* caster = Cast<ASoulShardsCharacter>(playerController->GetPawn());
		if (caster)
		{
			UPlayerInputController* inputController = caster->GetInputController();
			if (inputController)
			{
				inputController->SetInputMask2(inputMask);
			}
		}
	}
}

ACameraActor* ABattleArenaLevelScriptActor::GetReplayCameraActor()
{
	if (!SoulShardsBaseHelper::IsValidActor(this->ReplayCamera))
	{
		TArray<AActor*> results;
		UGameplayStatics::GetAllActorsWithTag(this, FName("ReplayCamera"), results);
		if (results.Num() > 0)
		{
			this->ReplayCamera = Cast<AReplayCamera>(results[0]);
		}
		else
		{
			this->ReplayCamera = this->GetWorld()->SpawnActor<AReplayCamera>(AReplayCamera::StaticClass(), FTransform::Identity);
			check(this->ReplayCamera);
		}
	}

	return this->ReplayCamera;
}

bool ABattleArenaLevelScriptActor::IsBeta()
{
	if (SteamApps() != nullptr)
	{
		AppId_t devAppId = 871820;
		bool isDevDlcInstalled = SteamApps()->BIsDlcInstalled(devAppId);
		if (!isDevDlcInstalled)
		{
			AppId_t appId = 903960;
			uint32 purchaseUnixTime = SteamApps()->GetEarliestPurchaseUnixTime(appId);
			if (purchaseUnixTime > 0)
			{
				return true;
			}
		}
	}

	return false;
}

void ABattleArenaLevelScriptActor::CheckVersionCompatibility()
{
	AHttpEnabledPlayerController* playerController = Cast<AHttpEnabledPlayerController>(this->GetWorld()->GetFirstPlayerController());
	if (playerController)
	{
		FCheckVersionCompatibilityEvent responseCallback;
		responseCallback.BindDynamic(this, &ABattleArenaLevelScriptActor::OnCheckVersionCompatibilityCompleted);
	
		playerController->GetHttpCallManager()->HttpCheckVersionCompatibility(responseCallback);
	}
}

void ABattleArenaLevelScriptActor::OnCheckVersionCompatibilityCompleted(bool isCompatible)
{
	if (isCompatible)
	{
		const FName defaultOnlineSubsystemName = UOnlineEngineInterface::Get()->GetDefaultOnlineSubsystemName();
		const FName playFabOnlineSubsystemName = "PlayFab";
		if ((this->GetWorld()->GetNetMode() != ENetMode::NM_DedicatedServer) && (defaultOnlineSubsystemName.Compare(playFabOnlineSubsystemName)) == 0)
		{
			// This fetches the clientAPI object from the PlayFab plugin, so you can make API calls with it.
			PlayFabClientPtr clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
			if (!clientAPI->IsClientLoggedIn())
			{
				FString cmdVal = FCommandLine::Get();
				if (FParse::Value(FCommandLine::Get(), TEXT("-xsolla-login-token"), cmdVal))
				{
					USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(this->GetWorld()->GetGameInstance());
					check(gameInstance);

					FString cusmtomUserId = gameInstance->GetCusmtomUserId();
					if (cusmtomUserId.IsEmpty())
					{
						FCustomLoginReceivedEvent customLoginReceivedEvent;
						customLoginReceivedEvent.BindDynamic(this, &ABattleArenaLevelScriptActor::OnCustomLoginReceived);

						FHttpFailureEvent failureEvent;
						AHttpEnabledPlayerController* playerController = Cast<AHttpEnabledPlayerController>(this->GetWorld()->GetFirstPlayerController());
						if (playerController)
						{
							failureEvent.BindDynamic(playerController, &AHttpEnabledPlayerController::OnGameClientHttpFailure);

							playerController->GetHttpCallManager()->HttpCustomLogin(customLoginReceivedEvent, failureEvent);
						}
					}
					else
					{
						this->OnCustomLoginReceived(cusmtomUserId);
					}
				}
				else
				{
					if (SteamUser())
					{
						PlayFab::ClientModels::FLoginWithSteamRequest request = AHttpEnabledPlayerController::CreateLoginWithSteamRequest();
						this->LoginWithPlayFab("steam", "steam", request.SteamTicket);
					}
				}
			}
			else
			{
				this->OnLoginCompleted();
			}
		}
		else
		{
#if WITH_EDITOR
			this->OnLoginCompleted();
			return;
#endif
		}
	}
	else
	{
		FUserDialogResponseEvent responseCallback;
		AHttpEnabledPlayerController* playerController = Cast<AHttpEnabledPlayerController>(this->GetWorld()->GetFirstPlayerController());
		if (playerController)
		{
			responseCallback.BindDynamic(playerController, &AHttpEnabledPlayerController::OnHttpConnectionErrorUserResponseReceived);

			playerController->ShowUserConfirmationDialog(
                TEXT("Error"),
                TEXT("Can't connect to game servers because the client is not up to date.  Please update to the latest version of the game."),
                TEXT(""),
                false,
                TEXT("Exit game"),
                true,
                TEXT(""),
                false,
                responseCallback,
                nullptr);
		}
	}
}

void ABattleArenaLevelScriptActor::OnCustomLoginReceived(FString customId)
{
	USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(this->GetWorld()->GetGameInstance());
	check(gameInstance);

	if (gameInstance->GetCusmtomUserId().IsEmpty())
	{
		gameInstance->SetCusmtomUserId(customId);
	}

	this->LoginWithPlayFab("customid", "customid", customId);
}

void ABattleArenaLevelScriptActor::LoginWithPlayFab(FString id, FString type, FString token)
{
	// This fetches the clientAPI object from the PlayFab plugin, so you can make API calls with it.
	PlayFabClientPtr clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	if (!clientAPI->IsClientLoggedIn())
	{

		const FName defaultOnlineSubsystemName = UOnlineEngineInterface::Get()->GetDefaultOnlineSubsystemName();
		IOnlineSubsystem* PlatformOnlineSub = IOnlineSubsystem::Get(defaultOnlineSubsystemName);
		IOnlineIdentityPtr onlineIdentity = PlatformOnlineSub->GetIdentityInterface();

		if (onlineIdentity.IsValid())
		{
			AHttpEnabledPlayerController* playerController = Cast<AHttpEnabledPlayerController>(this->GetWorld()->GetFirstPlayerController());
			if (playerController)
			{
				const int32 ControllerId = CastChecked<ULocalPlayer>(playerController->Player)->GetControllerId();

				if (!onlineIdentity->OnLoginCompleteDelegates[ControllerId].IsBoundToObject(this))
				{
					FOnLoginCompleteDelegate loginCompleteDelegate;
					loginCompleteDelegate.BindUObject(this, &ABattleArenaLevelScriptActor::OnLoginCompleted);

					OnLoginCompleteDelegateHandle = onlineIdentity->AddOnLoginCompleteDelegate_Handle(ControllerId, loginCompleteDelegate);
				}

				FOnlineAccountCredentials* credentials = new FOnlineAccountCredentials();
				credentials->Id = id;
				credentials->Type = type;
				credentials->Token = token;

				onlineIdentity->Login(ControllerId, *credentials);
			}
		}
	}
	else
	{
		this->OnLoginCompleted();
	}
}

void ABattleArenaLevelScriptActor::OnLoginCompleted(int32 LoginLocalUserNum, bool bLoginWasSuccessful, const FUniqueNetId& LoginUserId, const FString& LoginError)
{
	const FName defaultOnlineSubsystemName = UOnlineEngineInterface::Get()->GetDefaultOnlineSubsystemName();
	IOnlineSubsystem* PlatformOnlineSub = IOnlineSubsystem::Get(defaultOnlineSubsystemName);
	IOnlineIdentityPtr onlineIdentity = PlatformOnlineSub->GetIdentityInterface();

	if (onlineIdentity.IsValid())
	{
		onlineIdentity->ClearOnLoginCompleteDelegate_Handle(LoginLocalUserNum, OnLoginCompleteDelegateHandle);
	}

	AHttpEnabledPlayerController* playerController = Cast<AHttpEnabledPlayerController>(this->GetWorld()->GetFirstPlayerController());

	if (bLoginWasSuccessful)
	{
		this->OnLoginSuccess(playerController);
	}
	else
	{
		this->OnLoginError(playerController);
	}
}


void ABattleArenaLevelScriptActor::OnLoginSuccess(AHttpEnabledPlayerController* playerController)
{
	// An API Result object will contain the requested information, according to the API called.
	// FLoginResult contains some basic information about the player, but for most users, login is simply a mandatory step before calling other APIs.
	playerController->ChangeMenuWidget(this->GetProvisionPlayerWidgetClass());

	FProvisionData provisionData;
	provisionData.Realm = EGameRealm::EGDIceVoid;
	provisionData.OwnsIceVoidBetaDLC = this->IsBeta();
	FSubmitProvisionReceivedEvent submitProvisionReceivedEvent;
	submitProvisionReceivedEvent.BindDynamic(this, &ABattleArenaLevelScriptActor::OnSubmitProvisionReceived);

	FHttpFailureEvent failureEvent;
	failureEvent.BindDynamic(playerController, &AHttpEnabledPlayerController::OnGameClientHttpFailure);

	playerController->GetHttpCallManager()->HttpSubmitProvisionUser(provisionData, submitProvisionReceivedEvent, failureEvent);
	this->CreateUserTitleDisplayNameForSteam();
}

void ABattleArenaLevelScriptActor::OnSubmitProvisionReceived(FProvisionData submittedData)
{
	UE_LOG(SoulShardsLog, Log, TEXT("ABattleArenaLevelScriptActor::OnSubmitProvisionReceived: OnSubmitProvisionReceived() started."));
	this->OnLoginCompleted();
}

void ABattleArenaLevelScriptActor::OnLoginError(AHttpEnabledPlayerController* playerController) const
{
	// API calls can fail for many reasons, and you should always attempt to handle failure.
	// Why API calls fail (In order of likelihood):
	// PlayFabSettings.TitleId is not set. If you forget to set titleId to your title, then nothing will work.
	// Request parameters. If you have not provided the correct or required information for a particular API call, then it will fail. See error.errorMessage, error.errorDetails, or error.GenerateErrorReport() for more info.
	// Device connectivity issue. Cell-phones lose/regain connectivity constantly, and so any API call at any time can fail randomly, and then work immediately after. Going into a tunnel can disconnect you completely.
	// PlayFab server issue. As with all software, there can be issues.
	// The internet is not 100% reliable. Sometimes the message is corrupted or fails to reach the PlayFab server
	playerController->OnGameClientHttpFailure(EHttpFailureReason::HFRDisconnect);
}

void ABattleArenaLevelScriptActor::CreateUserTitleDisplayNameForSteam(bool retry)
{
	if (!SoulShardsHelper::IsSteamUser())
	{
		return;
	}

	FString titleDisplayName;
	AHttpEnabledPlayerController* playerController = Cast<AHttpEnabledPlayerController>(this->GetWorld()->GetFirstPlayerController());

	if (playerController)
	{
		// Gets the subsystem player token.
		const int32 controllerId = CastChecked<ULocalPlayer>(playerController->Player)->GetControllerId();
		const FName defaultOnlineSubsystemName = UOnlineEngineInterface::Get()->GetDefaultOnlineSubsystemName();
		IOnlineSubsystem* platformOnlineSub = IOnlineSubsystem::Get(defaultOnlineSubsystemName);
		IOnlineIdentityPtr onlineIdentity = platformOnlineSub->GetIdentityInterface();
		titleDisplayName = onlineIdentity->GetPlayerNickname(controllerId);

		int titleDisplayNameSize = titleDisplayName.Len();

		FString steamDisplayName = SteamFriends() ? SteamFriends()->GetPersonaName() : FString(TEXT(""));

		FString reducedSteamDisplayName = steamDisplayName.Len() > 25 ? steamDisplayName.Left(25) : steamDisplayName;

		// Creates user title display name with steam name (only first time).
		if (titleDisplayNameSize == 0)
		{
			PlayFab::ClientModels::FUpdateUserTitleDisplayNameRequest request;
			request.DisplayName = reducedSteamDisplayName;

			// This fetches the clientAPI object from the PlayFab plugin, so you can make API calls with it.
			PlayFabClientPtr clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
			clientAPI->UpdateUserTitleDisplayName(request,
                PlayFab::UPlayFabClientAPI::FUpdateUserTitleDisplayNameDelegate::CreateLambda(
                    [&](const PlayFab::ClientModels::FUpdateUserTitleDisplayNameResult& result)
            {

            }),
                PlayFab::FPlayFabErrorDelegate::CreateLambda(
                    [&](const PlayFab::FPlayFabCppError & ErrorResult)
            {
                if (retry)
                {
                    PlayFab::ClientModels::FLoginWithSteamRequest request = AHttpEnabledPlayerController::CreateLoginWithSteamRequest();
                    clientAPI->LoginWithSteam(
                        request,
                        PlayFab::UPlayFabClientAPI::FLoginWithSteamDelegate::CreateLambda(
                            [&](const PlayFab::ClientModels::FLoginResult& result)
                    {
                        this->CreateUserTitleDisplayNameForSteam(false /*retry*/);
                    }),
                        PlayFab::FPlayFabErrorDelegate::CreateLambda(
                            [&](const PlayFab::FPlayFabCppError & ErrorResult)
                    {

                    }));
                }
            }));
		}
	}
}

void ABattleArenaLevelScriptActor::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	this->TryPlayAutomatedCameraScript();

	this->TryPlayIntro();
}

void ABattleArenaLevelScriptActor::OnFinishedPlayingOutro()
{
	this->LevelState = ELevelState::ELSOutroPlayed;
}

void ABattleArenaLevelScriptActor::OnPlayerRespawned()
{

}

FReplayCameraScriptStepsData ABattleArenaLevelScriptActor::GetCameraScript(FString scriptName)
{
	FReplayCameraScriptStepsData toReturn;
	USoulShardsGameInstance* gameInstance = static_cast<USoulShardsGameInstance*>(this->GetWorld()->GetGameInstance());
	if (gameInstance)
	{
		FReplayCameraScriptStepsData* script = gameInstance->GetCameraScriptContainer()->GetScriptByName(scriptName);
		if (script)
		{
			toReturn = *script;
		}
	}

	return toReturn;
}

void ABattleArenaLevelScriptActor::OnAutomatedCameraScriptFinishedPlaying(FString scriptName)
{
	if (scriptName == "LevelIntro")
	{
		this->introLoopsPlayed++;
		UWorld* world = this->GetWorld();

		bool isPlayingReplay = SoulShardsHelper::IsPlayingReplay(world);
		AReplayCamera* replayCamera = Cast<AReplayCamera>(this->GetReplayCameraActor());
		FViewTargetTransitionParams params;

		TArray<ATotemCharacter*> totems;
		for (TActorIterator<ATotemCharacter> it(this->GetWorld()); it; ++it)
		{
			totems.Add(*it);
		}

		// If there are enough totems we proceed to totem intro
		if (totems.Num() >= 2 && !(this->introLoopsPlayed < this->LevelData->MinLoopLevelIntroIterationCount))
		{
			replayCamera->PlayAutoamtedCameraScript(this->GetCameraScript("TotemIntro"), params);
			LevelState = ELevelState::ELSPlayingPlayerIntro;
		}
		else
		{
			TArray<AActor*> cameras;
			UGameplayStatics::GetAllActorsWithTag(this->GetWorld(), "StartCamera", cameras);

			if (cameras.Num() > 0)
			{
				TArray<UActorComponent*> components = replayCamera->GetComponentsByClass(UCameraComponent::StaticClass());
				UCameraComponent* cameraComponent = Cast<UCameraComponent>(components[0]);
				FTransform replayCameraTransform = cameraComponent->GetComponentTransform();;
				cameras[0]->SetActorTransform(replayCameraTransform);
			}

			replayCamera->PlayAutoamtedCameraScript(this->GetCameraScript("LevelIntro"), params);
		}		
	} 
	else if (scriptName == "TotemIntro")
	{
		UWorld* world = this->GetWorld();

		TArray<ATotemCharacter*> totems;
		for (TActorIterator<ATotemCharacter> it(this->GetWorld()); it; ++it)
		{
			totems.Add(*it);
		}

		for (auto& totem: totems)
		{
			ASoulShardsCharacter* caster = totem->GetSoulCaster();
			if (caster && caster->HasAuthority())
			{
				UPlayerInputController* inputController = caster->GetInputController();
				if (inputController)
				{
					inputController->SetInputMask((int)EIMAllActions);
					inputController->SetInputMask2((int)EIMAllActions2);
				}
			}
		}

		ABattleArenaPlayerController* controller = this->GetFirstBattleArenaPlayerController();
		if (controller)
		{
			controller->SetViewTarget(controller->GetPawn());
			controller->ShowHudWidget();
			controller->PlayerCameraManager->bClientSimulatingViewTarget = false;
		}

		LevelState = ELevelState::ELSIntroPlayed;
	}
}

ELevelState ABattleArenaLevelScriptActor::GetLevelState() const
{
	return this->LevelState;
}

TSubclassOf<UUserWidget> ABattleArenaLevelScriptActor::GetProvisionPlayerWidgetClass()
{
	if (this->ProvisionPlayerWidgetClass.IsNull())
	{
		this->ProvisionPlayerWidgetClass = SoulShardsHelper::GetClassByString("/Game/UI/UI_ProvisionMessageScreen", this);
	}

	return this->ProvisionPlayerWidgetClass.LoadSynchronous();
}

ADynamicObjectPool* ABattleArenaLevelScriptActor::GetDynamicObjectPool()
{
	if (this->DynamicObjectPool == nullptr)
	{
		this->DynamicObjectPool = this->GetWorld()->SpawnActor<ADynamicObjectPool>();
	}
	
	return this->DynamicObjectPool;
}

ULevelData* ABattleArenaLevelScriptActor::GetLevelData() const
{
	return this->LevelData;
}

void ABattleArenaLevelScriptActor::BeginDestroy()
{
	if (IsValid(this->DynamicObjectPool))
	{
		this->DynamicObjectPool->Destroy();
	}

	Super::BeginDestroy();
}
