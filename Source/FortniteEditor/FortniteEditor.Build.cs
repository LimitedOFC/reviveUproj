// Copyright FNGameProj. All Rights Reserved.
using UnrealBuildTool;

public class FortniteEditor : ModuleRules
{
	public FortniteEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "UnrealEd",
			"FortniteGame", "PropertyEditor", "AssetTools",
			"ContentBrowser", "Slate", "SlateCore"
		});
	}
}
