// Copyright FNGameProj. All Rights Reserved.
using UnrealBuildTool;

public class FortniteVersion : ModuleRules
{
	public FortniteVersion(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core" });
	}
}
