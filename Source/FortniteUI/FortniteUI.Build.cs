// Copyright FNGameProj. All Rights Reserved.
using UnrealBuildTool;

public class FortniteUI : ModuleRules
{
	public FortniteUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "UMG", "Slate", "SlateCore",
			"FortniteGame", "GameplayTags", "InputCore"
		});
	}
}
