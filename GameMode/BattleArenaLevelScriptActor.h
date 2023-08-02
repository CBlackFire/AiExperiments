// <copyright file="BattleArenaLevelScriptActor.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "CancelableTimerProviderInterface.h"
#include "Player/Data/CameraTransitionData.h"
#include "Engine/PostProcessVolume.h"
#include "Player/Data/CrossHairData.h"
#include "Player/Data/ProvisionData.h"
#include "Player/Actor/ReplayCamera.h"
#include "Data/LevelData.h"
#include "GameMode/SoulShardsBaseLevelScriptActor.h"
#include "BattleArenaLevelScriptActor.generated.h"

UCLASS(Config = Game, Blueprintable, BlueprintType)
/// <summary>
/// Class that represents a battle arena level
/// </summary>
/// <seealso cref="ALevelScriptActor" />
class SOULSHARDS_API ABattleArenaLevelScriptActor : public ASoulShardsBaseLevelScriptActor
{
	GENERATED_BODY()
	
private:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ABattleArenaLevelScriptActor, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Level Data.
	/// </summary>
	ULevelData* LevelData;

	UPROPERTY(transient)
	/// <summary>
	/// The provision player widget class
	/// </summary>
	TSoftClassPtr<UUserWidget> ProvisionPlayerWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Post process settings volume.  Used to affect post process settings in the game and editor.
	/// </summary>
	APostProcessVolume* PostProcessVolume;

	UPROPERTY(transient, BlueprintReadOnly, Category = ABattleArenaLevelScriptActor, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// Overall brightness of the level.  Used to control intensity of glow and other lighting features.
	/// </summary>
	float BrightnessLevel;

	UPROPERTY(transient, BlueprintReadOnly, Category = ABattleArenaLevelScriptActor, meta = (AllowPrivateAccess = "true"))
	/// <summary>
	/// State of the level.
	/// </summary>
	ELevelState LevelState;

	UPROPERTY()
	/// <summary>
	/// Camera used for replay and action following
	/// </summary>
	AReplayCamera* ReplayCamera;	

	UPROPERTY()
	/// <summary>
	/// Camera that will be used when begin play is called
	/// </summary>
	FString StartCamera;

	/// <summary>
	/// The delegate handle for the login completed.
	/// </summary>
	FDelegateHandle OnLoginCompleteDelegateHandle;

	/// <summary>
	/// Numer of intro loops played.
	/// </summary>
	int introLoopsPlayed;

	/// <summary>
	/// Checks if it is a beta user.
	/// </summary>
	bool IsBeta();

	/// <summary>
	/// Gets a camera script
	/// </summary>
	FReplayCameraScriptStepsData GetCameraScript(FString scriptName);

	/// <summary>
	/// Checks if it is required to get the latest version of the client.
	/// </summary>
	void CheckVersionCompatibility();

	/// <summary>
	/// Gets the provision player widget class.
	/// </summary>
	TSubclassOf<UUserWidget> GetProvisionPlayerWidgetClass();

	UFUNCTION()
	/// <summary>
	/// The asynchronous callback that will be invoked after CheckVersionCompatitibility completes.
	/// </summary>
	/// <param name="isCompatible">Value indicating it the version is compatible with the web server.</param>
	void OnCheckVersionCompatibilityCompleted(bool isCompatible);

	/// <summary>
	/// Login with playfab online subsytem.
	/// </summary>
	/// <param name="id">Id of the user logging in (email, display name, facebook id, etc).</param>
	/// <param name="type">Type of account. Needed to identity the auth method to use (epic, internal, facebook, etc).</param>
	/// <param name="token">Credentials of the user logging in (password or auth token).</param>
	void LoginWithPlayFab(FString id, FString type, FString token);

	/// <summary>
	/// The asynchronous callback that will be invoked after PlayFab LoginWithCustomID completes successfully.
	/// </summary>
	void OnLoginSuccess(class AHttpEnabledPlayerController* playerController);

	/// <summary>
	/// The asynchronous callback that will be invoked after PlayFab LoginWithCustomID fails.
	/// </summary>
	void OnLoginError(class AHttpEnabledPlayerController* playerController) const;

	UFUNCTION()
	/// <summary>
	/// Called when the http custom login is completed.
	/// </summary>
	void OnCustomLoginReceived(FString customId);

	UFUNCTION()
	/// <summary>
	/// Called when the http submit provision user is completed.
	/// </summary>
	void OnSubmitProvisionReceived(FProvisionData submittedData);

	/// <summary>
	/// Creates user title display name with steam name (only first time).
	/// </summary>
	/// <param name="retry">True if should retry in fail, otherwise false.</param>
	void CreateUserTitleDisplayNameForSteam(bool retry = true);

	/// <summary>
	/// Sets the view camera to a start position
	/// </summary>
	void SetViewCameraToStart(UWorld* world) const;

	/// <summary>
	/// Called when online identity finishes to login.
	/// </summary>
	void OnLoginCompleted(int32 LoginLocalUserNum, bool bLoginWasSuccessful, const FUniqueNetId& LoginUserId, const FString& LoginError);

	/// <summary>
	/// Gets the first player controller, taking into account if this is a dedicated server or a local multiplayer scenario.
	/// </summary>
	class ABattleArenaPlayerController* GetFirstBattleArenaPlayerController() const;

	/// <summary>
	/// Tries to play the intro sequence (if there are player controllers).
	/// </summary>
	/// <returns>
	/// True if the intro started playing
	/// </returns>
	bool TryPlayIntro();

	/// <summary>
	/// Tries to plays a camera script.
	/// </summary>
	void TryPlayAutomatedCameraScript();

protected:

	/// <summary>
	/// Called when the game starts or when spawned
	/// </summary>
	virtual void BeginPlay() override;

	/// <summary>
	/// Called when it is decided that the object will be destroyed
	/// </summary>
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/// <summary>
	/// Called just after the map finishes loading.  Used to set initial camera.
	/// </summary>
	virtual void OnPostLoadMapWithWorld(UWorld* World) const override;

	/// <summary>
	/// Actor Tick
	/// </summary>
	/// <param name="DeltaTime">The delta time.</param>
	virtual void Tick(float DeltaTime) override;

public:	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Flag indicating whether automated intro should be played when loading map.
	/// </summary>
	bool PlayAutomatedIntroOnStart;

	UPROPERTY(Config, BlueprintReadOnly, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// The time in seconds that replay starts in.  Used to skip totem presentation time.
	/// </summary>
	int DefaultReplayStartTimeSec;

	UPROPERTY(Config, BlueprintReadOnly, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// The time in seconds that should pass before a player is asked to be queued
	/// </summary>
	float TimeBeforeQueueing;

	UPROPERTY(BlueprintReadOnly, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Dynamic object pool.
	/// </summary>
	class ADynamicObjectPool* DynamicObjectPool;

	/// <summary>
	/// Creates a new instance of <see cref="ABattleArenaLevelScriptActor"/> class
	/// </summary>
	ABattleArenaLevelScriptActor();

	UFUNCTION(BlueprintImplementableEvent, Category = UBattleArenaGamePhase)
	/// <summary>
	/// Called when the subsystem login is completed
	/// </summary>
	void OnLoginCompleted();

	UFUNCTION(BluePrintCallable, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Gets the material interfaces related to the specified base material interface.
	/// </summary>
	/// <returns>The array of material interfaces</returns>
	TArray<class UMaterialInterface*> GetMaterialInterfaces(UMaterialInterface* baseMaterialInterface);

	UFUNCTION(BluePrintCallable, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Triggers an event that can be handled by a phase
	/// </summary>
	void TriggerPhaseEvent(FString eventName, UObject* target = nullptr);

	UFUNCTION(BluePrintCallable, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Sets the local player input mask
	/// </summary>
	void SetLocalPlayerInputMask(UPARAM(meta = (Bitmask, BitmaskEnum = EPlayerInputMask)) int inputMask);

	UFUNCTION(BluePrintCallable, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Sets the local player input mask
	/// </summary>
	void SetLocalPlayerInputMask2(UPARAM(meta = (Bitmask, BitmaskEnum = EPlayerInputMask2)) int inputMask);

	UFUNCTION(BlueprintCallable, Category = AReplayPlayerController)
	/// <summary>
	/// Ensure camera actor.  Camera actor might get destroyed by the replay when time is changed.
	/// This can be called to make sure there is a camera actor available
	/// </summary>
	ACameraActor* GetReplayCameraActor();

	UFUNCTION(BluePrintImplementableEvent, Category = ABattleArenaLevelScriptActor)
	/// <summary>
	/// Called when a level event is generated.
	/// </summary>
	void OnLevelEventTriggered(FName levelEventName);

	UFUNCTION(BlueprintCallable, Category = AReplayPlayerController)
	/// <summary>
	/// Called when outro finishes playing. 
	/// </summary>
	void OnFinishedPlayingOutro();

	/// <summary>
	/// Notifies the level script actor that a player has respawned.
	/// </summary>
	void OnPlayerRespawned();

	/// <summary>
	/// Called when a camera script finishes playing.
	/// </summary>
	void OnAutomatedCameraScriptFinishedPlaying(FString scriptName);

	/// <summary>
	/// Gets the level state.
	/// </summary>
	ELevelState GetLevelState() const;

	/// <summary>
	/// Gets the dynamic object pool.
	/// </summary>
	class ADynamicObjectPool* GetDynamicObjectPool();

	ULevelData* GetLevelData() const;
	
	virtual void BeginDestroy() override;
};
