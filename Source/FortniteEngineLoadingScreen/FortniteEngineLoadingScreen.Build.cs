// Copyright FNGameProj. All Rights Reserved.
using UnrealBuildTool;

public class FortniteEngineLoadingScreen : ModuleRules
{
	public FortniteEngineLoadingScreen(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "MoviePlayer", "Slate", "SlateCore", "Engine"
		});
	}
}
