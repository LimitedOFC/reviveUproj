// Copyright FNGameProj. All Rights Reserved.

#include "Building/FortBuildingSettings.h"
#include "Building/FortBuildingComponent.h"

UFortBuildingSettings::UFortBuildingSettings()
{
	// Defaults already set via UPROPERTY initializers in the header.
	// Nothing extra needed here.
}

void UFortBuildingSettings::ApplyToComponent(UFortBuildingComponent* Component) const
{
	if (!Component) { return; }

	Component->ApplyBuildingSettings(this);
}

#if WITH_EDITOR
void UFortBuildingSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SaveConfig();
}
#endif
