// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ISeeMe : ModuleRules
{
	public ISeeMe(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "OnlineSubsystem", 
			"OnlineSubsystemSteam","GeometryCollectionEngine", "FieldSystemEngine", "OnlineSubsystemUtils",
			"Niagara"
        });
	}
}
