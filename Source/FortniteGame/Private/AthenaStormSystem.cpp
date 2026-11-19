// Copyright FNGameProj. All Rights Reserved.

#include "AthenaStormSystem.h"
#include "AthenaCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "Math/UnrealMathUtility.h"

AAthenaStormSystem::AAthenaStormSystem()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// Default S9 storm phases
	StormPhases.SetNum(8);
	StormPhases[0] = { 120.0f, 120.0f, 14000.0f, 0.5f  };
	StormPhases[1] = { 120.0f, 100.0f, 10000.0f, 1.0f  };
	StormPhases[2] = { 100.0f,  80.0f,  7000.0f, 2.0f  };
	StormPhases[3] = {  80.0f,  60.0f,  4000.0f, 4.0f  };
	StormPhases[4] = {  60.0f,  50.0f,  2000.0f, 8.0f  };
	StormPhases[5] = {  45.0f,  40.0f,   800.0f, 10.0f };
	StormPhases[6] = {  30.0f,  25.0f,   200.0f, 20.0f };
	StormPhases[7] = {  20.0f,  20.0f,    50.0f, 50.0f };
}

void AAthenaStormSystem::BeginPlay()
{
	Super::BeginPlay();
	SafeZoneCenter     = FVector::ZeroVector;
	NextSafeZoneCenter = FVector::ZeroVector;
	CurrentRadius      = MapRadius;
}

void AAthenaStormSystem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bStormStarted) { return; }

	PhaseTimeRemaining -= DeltaSeconds;

	if (bIsMoving && StormPhases.IsValidIndex(CurrentPhaseIndex))
	{
		// Interpolate circle toward next center and radius
		const float Total  = StormPhases[CurrentPhaseIndex].MoveTime;
		const float Elapsed = Total - FMath::Max(PhaseTimeRemaining, 0.0f);
		const float Alpha   = FMath::Clamp(Elapsed / Total, 0.0f, 1.0f);
		const float TargetR = StormPhases.IsValidIndex(CurrentPhaseIndex + 1)
		                    ? StormPhases[CurrentPhaseIndex + 1].Radius : 0.0f;

		SafeZoneCenter = FMath::Lerp(SafeZoneCenter, NextSafeZoneCenter, Alpha);
		CurrentRadius  = FMath::Lerp(StartMoveRadius, TargetR, Alpha);

		OnStormCircleUpdated.Broadcast(SafeZoneCenter, CurrentRadius);
	}

	if (PhaseTimeRemaining <= 0.0f)
	{
		AdvancePhase();
	}
}

void AAthenaStormSystem::StartStorm()
{
	bStormStarted = true;
	CurrentPhaseIndex = -1;
	AdvancePhase();
}

void AAthenaStormSystem::AdvancePhase()
{
	++CurrentPhaseIndex;
	if (!StormPhases.IsValidIndex(CurrentPhaseIndex)) { return; }

	OnStormPhaseChanged.Broadcast(CurrentPhaseIndex);
	BeginWaitPhase();
}

void AAthenaStormSystem::BeginWaitPhase()
{
	bIsMoving = false;
	PhaseTimeRemaining = StormPhases[CurrentPhaseIndex].WaitTime;

	// Determine next circle center now (shown on minimap during wait)
	float NextR = StormPhases.IsValidIndex(CurrentPhaseIndex + 1)
	            ? StormPhases[CurrentPhaseIndex + 1].Radius : 0.0f;
	NextSafeZoneCenter = PickNextCircleCenter(SafeZoneCenter, NextR);
}

void AAthenaStormSystem::BeginMovePhase()
{
	bIsMoving      = true;
	StartMoveRadius = CurrentRadius;
	PhaseTimeRemaining = StormPhases[CurrentPhaseIndex].MoveTime;

	GetWorldTimerManager().SetTimer(DamageTickHandle, this,
		&AAthenaStormSystem::TickStormDamage,
		StormDamageTickRate, true);
}

void AAthenaStormSystem::TickStormDamage()
{
	if (!StormPhases.IsValidIndex(CurrentPhaseIndex)) { return; }
	const float DPS = StormPhases[CurrentPhaseIndex].DamagePerSecond;

	for (TActorIterator<AAthenaCharacter> It(GetWorld()); It; ++It)
	{
		AAthenaCharacter* Char = *It;
		if (Char && Char->IsAlive() && IsActorInStorm(Char))
		{
			Char->ApplyHealthChange(-DPS * StormDamageTickRate, this);
		}
	}
}

bool AAthenaStormSystem::IsActorInStorm(AActor* TestActor) const
{
	if (!TestActor) { return false; }
	const FVector Loc = TestActor->GetActorLocation();
	const float Dist  = FVector::Dist2D(Loc, SafeZoneCenter);
	return Dist > CurrentRadius;
}

FVector AAthenaStormSystem::PickNextCircleCenter(FVector Current, float NextRadius) const
{
	const float MaxOffset = FMath::Max(CurrentRadius - NextRadius, 0.0f) * 0.7f;
	const float Angle     = FMath::RandRange(0.0f, 360.0f);
	const float Distance  = FMath::RandRange(0.0f, MaxOffset);
	return Current + FVector(
		FMath::Cos(FMath::DegreesToRadians(Angle)) * Distance,
		FMath::Sin(FMath::DegreesToRadians(Angle)) * Distance,
		0.0f);
}

void AAthenaStormSystem::ForceNextPhase()
{
	PhaseTimeRemaining = 0.0f;
}
