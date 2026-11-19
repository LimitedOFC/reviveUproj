// Copyright FNGameProj. All Rights Reserved.
// FortniteGame primary module header

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FFortniteGameModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
