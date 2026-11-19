// Copyright FNGameProj. All Rights Reserved.
using UnrealBuildTool;

public class EasyAntiCheatCommon : ModuleRules
{
	public EasyAntiCheatCommon(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		// Stub — no EAC SDK in this environment.
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });
	}
}
