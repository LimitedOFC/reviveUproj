// Copyright FNGameProj. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class FortniteGameTarget : TargetRules
{
	public FortniteGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.AddRange(new string[]
		{
			"FortniteGame",
			"FortniteUI",
			"FortniteVersion",
			"ArsenicCoreRuntime",
			"ArsenicCoreUI",
			"CommonDialogueRuntime",
			"FortniteConversationRuntime",
			"FortniteConversationUI",
			"AnalyticsBlueprintLibrary"
		});

		bUseLoggingInShipping = false;
	}
}
