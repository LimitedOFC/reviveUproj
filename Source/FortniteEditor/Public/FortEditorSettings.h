// Copyright FNGameProj. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FortEditorSettings.generated.h"

/**
 * UFortEditorSettings
 * Project-editor settings for FNGameProj — cosmetic preview defaults,
 * backport warnings and content browser display options.
 */
UCLASS(config = Editor, defaultconfig, meta = (DisplayName = "Fortnite Editor Settings"))
class FORTNITEEDITOR_API UFortEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UFortEditorSettings();

	/** Default outfit shown in the Cosmetic Preview map when none is selected. */
	UPROPERTY(config, EditAnywhere, Category = "Cosmetics|Preview")
	FString DefaultPreviewSkin = TEXT("Outfit_RamirezBase");

	/** Map to open automatically when the Cosmetic Preview button is pressed. */
	UPROPERTY(config, EditAnywhere, Category = "Cosmetics|Preview")
	FSoftObjectPath DefaultPreviewMap;

	/** Shows a backport-status badge on assets in the content browser. */
	UPROPERTY(config, EditAnywhere, Category = "Cosmetics|Backport")
	bool bShowBackportStatusInContentBrowser = true;

	/** Logs a warning on startup if any cosmetic has BackportState = Pending. */
	UPROPERTY(config, EditAnywhere, Category = "Cosmetics|Backport")
	bool bWarnOnPendingBackports = true;
};
