// Copyright FNGameProj. All Rights Reserved.
// AthenaStormSystem — manages storm phases, ring positions, and storm damage.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AthenaStormSystem.generated.h"

USTRUCT(BlueprintType)
struct FORTNITEGAME_API FAthenaStormPhase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) float WaitTime   = 120.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MoveTime   = 120.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Radius     = 14000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float DamagePerSecond = 0.5f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStormPhaseChanged, int32, NewPhaseIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStormCircleUpdated, FVector, NewCenter, float, NewRadius);

UCLASS(BlueprintType, Blueprintable)
class FORTNITEGAME_API AAthenaStormSystem : public AActor
{
	GENERATED_BODY()

public:
	AAthenaStormSystem();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Storm")
	void StartStorm();

	UFUNCTION(BlueprintCallable, Category = "Storm")
	void ForceNextPhase();

	UFUNCTION(BlueprintPure, Category = "Storm")
	int32 GetCurrentPhaseIndex() const { return CurrentPhaseIndex; }

	UFUNCTION(BlueprintPure, Category = "Storm")
	float GetCurrentPhaseTimeRemaining() const { return PhaseTimeRemaining; }

	UFUNCTION(BlueprintPure, Category = "Storm")
	float GetCurrentStormRadius() const { return CurrentRadius; }

	UFUNCTION(BlueprintPure, Category = "Storm")
	FVector GetSafeZoneCenter() const { return SafeZoneCenter; }

	UFUNCTION(BlueprintPure, Category = "Storm")
	bool IsMoving() const { return bIsMoving; }

	UFUNCTION(BlueprintPure, Category = "Storm")
	bool IsActorInStorm(AActor* TestActor) const;

	UPROPERTY(BlueprintAssignable, Category = "Storm|Events")
	FOnStormPhaseChanged OnStormPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Storm|Events")
	FOnStormCircleUpdated OnStormCircleUpdated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storm|Config")
	TArray<FAthenaStormPhase> StormPhases;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storm|Config")
	float MapRadius = 20000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storm|Config")
	float StormDamageTickRate = 0.5f;

protected:
	int32  CurrentPhaseIndex  = -1;
	float  PhaseTimeRemaining = 0.0f;
	bool   bIsMoving          = false;
	bool   bStormStarted      = false;

	FVector SafeZoneCenter;
	FVector NextSafeZoneCenter;
	float   CurrentRadius;
	float   StartMoveRadius;

	FTimerHandle DamageTickHandle;

	void AdvancePhase();
	void BeginWaitPhase();
	void BeginMovePhase();
	void TickStormDamage();
	FVector PickNextCircleCenter(FVector CurrentCenter, float NextRadius) const;
};
