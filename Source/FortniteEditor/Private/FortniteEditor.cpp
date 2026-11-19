// Copyright FNGameProj. All Rights Reserved.
#include "FortniteEditor.h"
#include "FortEditorSettings.h"
#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"

IMPLEMENT_MODULE(FFortniteEditorModule, FortniteEditor);

UFortEditorSettings::UFortEditorSettings()
{
}

void FFortniteEditorModule::StartupModule()
{
	// Register custom asset type actions, thumbnail renderers, detail customizations here
}

void FFortniteEditorModule::ShutdownModule()
{
}
