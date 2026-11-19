// Copyright FNGameProj. All Rights Reserved.
using UnrealBuildTool;

public class ArsenicCoreRuntime : ModuleRules
{
	public ArsenicCoreRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "FortniteGame",
			"GameplayAbilities", "GameplayTags"
		});
	}
}
