// <copyright file="MainMenuLevelScriptActor.h" company="Psiontech">
// Copyright (c) Psiontech SA, 2015 All Right Reserved, http://www.psiontech.com/
//
// Confidential Information of Psiontech SA. Not for disclosure or distribution without Psiontech's
// prior written consent. This software contains code, techniques and know-how which is confidential 
// and proprietary to Psiontech.
// </copyright>

#pragma once

#include "GameMode/SoulShardsBaseLevelScriptActor.h"
#include "MainMenuLevelScriptActor.generated.h"

UCLASS(Config = Game, Blueprintable, BlueprintType)
/// <summary>
/// Class that represents a main menu level
/// </summary>
/// <seealso cref="ALevelScriptActor" />
class SOULSHARDS_API AMainMenuLevelScriptActor : public ASoulShardsBaseLevelScriptActor
{
	GENERATED_BODY()

private:
	
	/// <summary>
	/// Sets the view camera to a start position
	/// </summary>
	void SetViewCameraToStart(UWorld* world) const;
	
protected:
	
	/// <summary>
	/// Called just after the map finishes loading.  Used to set initial camera.
	/// </summary>
	virtual void OnPostLoadMapWithWorld(UWorld* World) const override;

	/// <summary>
	/// Called when the game starts or when spawned
	/// </summary>
	virtual void BeginPlay() override;	

public:

	/// <summary>
	/// Initializes a new instance of the <see cref="AMainMenuLevelScriptActor"/> class.
	/// </summary>
	AMainMenuLevelScriptActor();

};
