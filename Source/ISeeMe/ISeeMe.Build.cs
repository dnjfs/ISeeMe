// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ISeeMe : ModuleRules
{
	public ISeeMe(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		bEnableUndefinedIdentifierWarnings = false;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "OnlineSubsystem", 
			"OnlineSubsystemSteam","OnlineSubsystemUtils", "Niagara", "GeometryCache",
        });
	}
}
