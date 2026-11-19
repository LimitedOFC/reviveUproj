// Copyright FNGameProj. All Rights Reserved.

#include "AthenaAIController.h"
#include "AthenaAIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "AthenaCharacter.h"

AAthenaAIController::AAthenaAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree"));
	BlackboardComponent   = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));

	FortPerceptionComponent = CreateDefaultSubobject<UAthenaAIPerceptionComponent>(TEXT("FortPerception"));
	SetPerceptionComponent(*FortPerceptionComponent);

	// Sight config
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius                = 2500.0f;
	SightConfig->LoseSightRadius            = 3000.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->SetMaxAge(5.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	FortPerceptionComponent->ConfigureSense(*SightConfig);

	// Hearing config
	UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 1500.0f;
	HearingConfig->SetMaxAge(3.0f);
	FortPerceptionComponent->ConfigureSense(*HearingConfig);

	FortPerceptionComponent->SetDominantSense(*SightConfig->GetSenseImplementation());
}

void AAthenaAIController::BeginPlay()
{
	Super::BeginPlay();
	FortPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAthenaAIController::OnPerceptionUpdated);
}

void AAthenaAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AAthenaAIController::OnUnPossess()
{
	Super::OnUnPossess();
	CurrentTarget.Reset();
}

void AAthenaAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		if (Actor && Actor != GetPawn())
		{
			FActorPerceptionBlueprintInfo Info;
			FortPerceptionComponent->GetActorsPerception(Actor, Info);
			bool bIsSensed = false;
			for (const FAIStimulus& Stimulus : Info.LastSensedStimuli)
			{
				if (Stimulus.WasSuccessfullySensed()) { bIsSensed = true; break; }
			}

			if (bIsSensed && !CurrentTarget.IsValid())
			{
				CurrentTarget = Actor;
			}
			else if (!bIsSensed && CurrentTarget == Actor)
			{
				CurrentTarget.Reset();
			}
		}
	}
	UpdateBlackboardValues();
}

void AAthenaAIController::UpdateBlackboardValues()
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsObject(FName("Target"), CurrentTarget.Get());
		BlackboardComponent->SetValueAsBool(FName("HasTarget"), CurrentTarget.IsValid());
	}
}

void AAthenaAIController::SetAIBehavior(EAIBehavior NewBehavior)
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsEnum(FName("CurrentBehavior"), (uint8)NewBehavior);
	}
}

void AAthenaAIController::MoveToSafeZone()
{
	SetAIBehavior(EAIBehavior::MovingToZone);
}
