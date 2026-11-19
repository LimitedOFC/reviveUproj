// Copyright FNGameProj. All Rights Reserved.
using UnrealBuildTool;

public class LocalNotifications : ModuleRules
{
	public LocalNotifications(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });
	}
}
