// Copyright FNGameProj. All Rights Reserved.
#include "AthenaAIPerceptionComponent.h"
#include "GameFramework/Actor.h"

UAthenaAIPerceptionComponent::UAthenaAIPerceptionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UAthenaAIPerceptionComponent::CanHearGunshot(AActor* Instigator, float SoundRadius) const
{
	if (!Instigator || !GetOwner()) { return false; }
	const float Dist = FVector::Dist(GetOwner()->GetActorLocation(), Instigator->GetActorLocation());
	return Dist <= SoundRadius;
}
