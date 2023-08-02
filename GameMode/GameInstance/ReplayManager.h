// <copyright file="ReplayManager.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "SoulShardsEnums.h"
#include "NetworkReplayStreaming.h"
#include "ReplayManager.generated.h"

USTRUCT(BlueprintType)
struct FS_ReplayInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString ReplayName;

	UPROPERTY(BlueprintReadOnly)
	FString FriendlyName;

	UPROPERTY(BlueprintReadOnly)
	FDateTime Timestamp;

	UPROPERTY(BlueprintReadOnly)
	int32 LengthInMS;

	UPROPERTY(BlueprintReadOnly)
	bool bIsValid;

	FS_ReplayInfo(FString NewName, FString NewFriendlyName, FDateTime NewTimestamp, int32 NewLengthInMS)
	{
		ReplayName = NewName;
		FriendlyName = NewFriendlyName;
		Timestamp = NewTimestamp;
		LengthInMS = NewLengthInMS;
		bIsValid = true;
	}

	FS_ReplayInfo()
	{
		ReplayName = "Replay";
		FriendlyName = "Replay";
		Timestamp = FDateTime::MinValue();
		LengthInMS = 0;
		bIsValid = false;
	}
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FFindReplaysEvent, const TArray<FS_ReplayInfo>&, replayList);


UCLASS(BlueprintType)
/// <summary>
/// Manages replays in a game isntance
/// </summary>
/// <seealso cref="UObject" />
class SOULSHARDS_API UReplayManager : public UObject
{
	GENERATED_BODY()
	
private:

	/// <summary>
	/// The parent game instance
	/// </summary>
	UGameInstance* parentGameInstance;

	/// <summary>
	/// Name of the last replay that was recorded
	/// </summary>
	FString lastRecordedReplayName;

	/// <summary>
	/// Name of the last replay that was played
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UReplayManager, meta = (AllowPrivateAccess = "true"))
	FString LastPlayedReplayName;

	// for FindReplays() 
	TSharedPtr<INetworkReplayStreamer> EnumerateStreamsPtr;	

	// for DeleteReplays(..)
	FDeleteFinishedStreamCallback OnDeleteFinishedStreamCompleteDelegate;

	void OnDeleteFinishedStreamComplete(const FDeleteFinishedStreamResult& streamResult);


public:

	/// <summary>
	/// Initializes a new instance of the <see cref="UReplayManager"/> class.
	/// </summary>
	UReplayManager();

	/// <summary>
	/// Gets the world
	/// </summary>
	virtual UWorld* GetWorld() const override;
	
	/// <summary>
	/// Gets the parent game instance
	/// </summary>
	UGameInstance* GetParentGameInstance() const;

	/// <summary>
	/// Sets the parent game instance
	/// </summary>
	virtual void SetParentGameInstance(UGameInstance* gameInstance);

	/// <summary>
/// Starts recording a replay for a game.
/// </summary>
	void StartRecordingGameReplay();

	/// <summary>
	/// Stops recording a replay for a game.
	/// </summary>
	void StopRecordingGameReplay() const;

	UFUNCTION(BlueprintCallable, Category = USoulShardsGameInstance)
	/// <summary>
	/// Replays the last recorded game
	/// </summary>
	void ReplayLastRecordedGame();

	UFUNCTION(BlueprintCallable, Category = UReplayManager)
	/// <summary>
	/// Start playback for a previously recorded Replay, from blueprint
	/// </summary>
	void PlayReplay(FString ReplayName);

	UFUNCTION(BlueprintCallable, Category = UReplayManager)
	/// <summary>
	/// Start looking for/finding replays on the hard drive
	/// </summary>
	void FindReplays(FFindReplaysEvent onReplaysFound) const;

	UFUNCTION(BlueprintCallable, Category = UReplayManager)
	/// <summary>
	/// Apply a new custom name to the replay (for UI only)
	/// </summary>
	void RenameReplay(const FString &ReplayName, const FString &NewFriendlyReplayName);

	UFUNCTION(BlueprintCallable, Category = UReplayManager)
	/// <summary>
	/// Delete a previously recorded replay
	/// </summary>
	void DeleteReplay(const FString &ReplayName);

	UFUNCTION(BlueprintCallable, Category = UReplayManager)
	/// <summary>
	/// True if a replay is being played
	/// </summary>
	bool IsPlayingReplay(UWorld* world);

	/// <summary>
	/// Gets the name of the last recorded replay
	/// </summary>
	FString GetLastRecordedReplayName();

	/// <summary>
	/// Gets the name of the last played replay
	/// </summary>
	FString GetLastPlayedReplayName();
};
