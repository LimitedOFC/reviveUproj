// Copyright FNGameProj. All Rights Reserved.
using UnrealBuildTool;

public class ArsenicUI : ModuleRules
{
	public ArsenicUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "UMG", "Slate", "SlateCore",
			"ArsenicRuntime", "ArsenicCoreUI"
		});
	}
}
