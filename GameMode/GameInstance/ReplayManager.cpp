// <copyright file="ReplayManager.cpp" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>


#include "SoulShards.h"
#include "GameMode/GameInstance/SoulShardsGameInstance.h"
#include "NetworkVersion.h"
#include "Runtime/NetworkReplayStreaming/NullNetworkReplayStreaming/Public/NullNetworkReplayStreaming.h"
#include "Engine/DemoNetDriver.h"
#include "ReplayManager.h"


UReplayManager::UReplayManager()
{
	this->parentGameInstance = nullptr;

	// create a ReplayStreamer for FindReplays() and DeleteReplay(..)
	EnumerateStreamsPtr = FNetworkReplayStreaming::Get().GetFactory().CreateReplayStreamer();
	// Link DeleteReplay() delegate to function	
	OnDeleteFinishedStreamCompleteDelegate = FDeleteFinishedStreamCallback::CreateUObject(this, &UReplayManager::OnDeleteFinishedStreamComplete);

}


UWorld* UReplayManager::GetWorld() const
{
	if (this->parentGameInstance)
	{
		return this->parentGameInstance->GetWorld();
	}

	return nullptr;
}

UGameInstance* UReplayManager::GetParentGameInstance() const
{
	return this->parentGameInstance;
}

void UReplayManager::SetParentGameInstance(UGameInstance* gameInstance)
{
	this->parentGameInstance = gameInstance;
}

void UReplayManager::StartRecordingGameReplay()
{	
	if (this->parentGameInstance)
	{
		UDemoNetDriver* demoNetdriver = this->GetWorld()->GetDemoNetDriver();
		if (demoNetdriver == nullptr || !demoNetdriver->IsRecording())
		{
			this->lastRecordedReplayName = FGuid::NewGuid().ToString();
			TArray<FString> options;
			//options.Add("ReplayStreamerOverride=InMemoryNetworkReplayStreaming");
			options.Add("ReplayStreamerOverride=LocalFileNetworkReplayStreaming");
			this->parentGameInstance->StartRecordingReplay(this->lastRecordedReplayName, TEXT(""), options);
		}
	}
}

void UReplayManager::StopRecordingGameReplay() const
{
	if (this->parentGameInstance)
	{
		this->parentGameInstance->StopRecordingReplay();
	}
}

void UReplayManager::ReplayLastRecordedGame()
{
	if (this->parentGameInstance)
	{
		if (!this->lastRecordedReplayName.IsEmpty())
		{
			this->PlayReplay(this->lastRecordedReplayName);
		}
	}
}

void UReplayManager::PlayReplay(FString replayName)
{
	this->LastPlayedReplayName = replayName;
	this->parentGameInstance->PlayReplay(replayName);
}

void UReplayManager::FindReplays(FFindReplaysEvent onReplaysFound) const
{
	if (this->EnumerateStreamsPtr.Get())
	{
		FEnumerateStreamsCallback onEnumerateStreamsCompleteDelegate;
		onEnumerateStreamsCompleteDelegate.BindLambda([=](const FEnumerateStreamsResult& result)
		{				
			TArray<FS_ReplayInfo> AllReplays;

			for (FNetworkReplayStreamInfo StreamInfo : result.FoundStreams)
			{
				if (!StreamInfo.bIsLive)
				{
					AllReplays.Add(FS_ReplayInfo(StreamInfo.Name, StreamInfo.FriendlyName, StreamInfo.Timestamp, StreamInfo.LengthInMS));
				}
			}

			onReplaysFound.ExecuteIfBound(AllReplays);
		});

		const TArray<FString> extraParms;
		this->EnumerateStreamsPtr.Get()->EnumerateStreams(FNetworkReplayVersion(), 0, FString(), extraParms, onEnumerateStreamsCompleteDelegate);
	}
}

void UReplayManager::RenameReplay(const FString &ReplayName, const FString &NewFriendlyReplayName)
{
	// Get File Info
	FNullReplayInfo Info;

	const FString DemoPath = FPaths::Combine(*FPaths::ProjectSavedDir(), TEXT("Demos/"));
	const FString StreamDirectory = FPaths::Combine(*DemoPath, *ReplayName);
	const FString StreamFullBaseFilename = FPaths::Combine(*StreamDirectory, *ReplayName);
	const FString InfoFilename = StreamFullBaseFilename + TEXT(".replayinfo");

	TUniquePtr<FArchive> InfoFileArchive(IFileManager::Get().CreateFileReader(*InfoFilename));

	if (InfoFileArchive.IsValid() && InfoFileArchive->TotalSize() != 0)
	{
		FString JsonString;
		*InfoFileArchive << JsonString;

		Info.FromJson(JsonString);
		Info.bIsValid = true;

		InfoFileArchive->Close();
	}

	// Set FriendlyName
	Info.FriendlyName = NewFriendlyReplayName;

	// Write File Info
	TUniquePtr<FArchive> ReplayInfoFileAr(IFileManager::Get().CreateFileWriter(*InfoFilename));

	if (ReplayInfoFileAr.IsValid())
	{
		FString JsonString = Info.ToJson();
		*ReplayInfoFileAr << JsonString;

		ReplayInfoFileAr->Close();
	}
}
void UReplayManager::DeleteReplay(const FString &ReplayName)
{
	if (this->EnumerateStreamsPtr.Get())
	{
		this->EnumerateStreamsPtr.Get()->DeleteFinishedStream(ReplayName, OnDeleteFinishedStreamCompleteDelegate);
	}
}

void UReplayManager::OnDeleteFinishedStreamComplete(const FDeleteFinishedStreamResult& streamResult)
{
	//FindReplays();
}

FString UReplayManager::GetLastRecordedReplayName()
{
	return this->lastRecordedReplayName;
}

FString UReplayManager::GetLastPlayedReplayName()
{
	return this->LastPlayedReplayName;
}

bool UReplayManager::IsPlayingReplay(UWorld* world)
{
	if (world)
	{
		if (world->GetDemoNetDriver())
		{
			return world->GetDemoNetDriver()->IsPlaying();
		}
	}

	return false;
}
