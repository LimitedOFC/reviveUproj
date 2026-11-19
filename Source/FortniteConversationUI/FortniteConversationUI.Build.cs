// Copyright FNGameProj. All Rights Reserved.
using UnrealBuildTool;

public class FortniteConversationUI : ModuleRules
{
	public FortniteConversationUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "UMG", "Slate", "SlateCore",
			"FortniteConversationRuntime"
		});
	}
}
