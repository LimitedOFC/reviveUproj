// Copyright FNGameProj. All Rights Reserved.
using UnrealBuildTool;

public class AnalyticsBlueprintLibrary : ModuleRules
{
	public AnalyticsBlueprintLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "Analytics", "AnalyticsET"
		});
	}
}
