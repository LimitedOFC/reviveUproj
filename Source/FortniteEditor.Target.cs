// Copyright FNGameProj. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class FortniteEditorTarget : TargetRules
{
	public FortniteEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.AddRange(new string[]
		{
			"FortniteGame",
			"FortniteUI",
			"FortniteEditor",
			"FortniteVersion",
			"ArsenicCoreRuntime",
			"ArsenicCoreUI",
			"CommonDialogueRuntime",
			"FortniteConversationRuntime",
			"FortniteConversationUI",
			"AnalyticsBlueprintLibrary"
		});

		PostBuildSteps.Add("if not exist \"$(ProjectDir)\\x64\" mkdir \"$(ProjectDir)\\x64\"");
		PostBuildSteps.Add("copy /Y \"$(ProjectDir)\\Binaries\\Win64\\UE4Editor-*.dll\" \"$(ProjectDir)\\x64\\\"");
	}
}
