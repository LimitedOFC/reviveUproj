// Copyright FNGameProj. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "AthenaAIPerceptionComponent.generated.h"

/** Fort-specific AI perception extension. */
UCLASS(ClassGroup = AI, meta = (BlueprintSpawnableComponent))
class FORTNITEGAME_API UAthenaAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()
public:
	UAthenaAIPerceptionComponent(const FObjectInitializer& ObjectInitializer);

	/** Returns true if the given actor is within hearing range of a gunshot. */
	UFUNCTION(BlueprintCallable, Category = "Perception")
	bool CanHearGunshot(AActor* Instigator, float SoundRadius) const;
};
