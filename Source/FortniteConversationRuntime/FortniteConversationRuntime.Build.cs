// Copyright FNGameProj. All Rights Reserved.
using UnrealBuildTool;

public class FortniteConversationRuntime : ModuleRules
{
	public FortniteConversationRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "FortniteGame",
			"CommonDialogueRuntime", "GameplayTags"
		});
	}
}
