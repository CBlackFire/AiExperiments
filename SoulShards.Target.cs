// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SoulShardsTarget : TargetRules
{
	public SoulShardsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
        bUsesSteam = true;
         
        
		
		ExtraModuleNames.Add("SoulShards");
        ExtraModuleNames.Add("SoulShardsBase");
    }
}
