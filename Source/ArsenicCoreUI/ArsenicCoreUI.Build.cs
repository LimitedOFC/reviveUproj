// Copyright FNGameProj. All Rights Reserved.
using UnrealBuildTool;

public class ArsenicCoreUI : ModuleRules
{
	public ArsenicCoreUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "UMG", "Slate", "SlateCore",
			"ArsenicCoreRuntime", "FortniteGame"
		});
	}
}
