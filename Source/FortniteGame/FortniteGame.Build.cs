// Copyright FNGameProj. All Rights Reserved.
// FortniteGame Module Build Rules

using UnrealBuildTool;
using System.Collections.Generic;

public class FortniteGame : ModuleRules
{
	public FortniteGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"AIModule",
			"UMG",
			"Niagara",
			"NavigationSystem",
			"OnlineSubsystem",
			"OnlineSubsystemUtils"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
			"RenderCore",
			"RHI",
			"AnimGraphRuntime",
			"MovieScene",
			"MovieSceneTracks",
			"LevelSequence"
		});

		// Allow Blueprint-only projects to still use this module
		DynamicallyLoadedModuleNames.AddRange(new string[] { });

		// Optimization flags
		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
	}
}
