// Copyright FNGameProj. All Rights Reserved.
using UnrealBuildTool;

public class ArsenicRuntime : ModuleRules
{
	public ArsenicRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "FortniteGame", "ArsenicCoreRuntime"
		});
	}
}
