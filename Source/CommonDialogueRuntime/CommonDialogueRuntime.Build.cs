// Copyright FNGameProj. All Rights Reserved.
using UnrealBuildTool;

public class CommonDialogueRuntime : ModuleRules
{
	public CommonDialogueRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "GameplayTags", "FortniteGame"
		});
	}
}
