// Copyright FNGameProj. All Rights Reserved.
#include "FortniteEngineLoadingScreen.h"
#include "MoviePlayer.h"

IMPLEMENT_MODULE(FFortniteEngineLoadingScreenModule, FortniteEngineLoadingScreen);

void FFortniteEngineLoadingScreenModule::StartupModule()
{
#if !WITH_EDITOR
	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
	LoadingScreen.MinimumLoadingScreenDisplayTime = 0.5f;
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
#endif
}

void FFortniteEngineLoadingScreenModule::ShutdownModule() {}
