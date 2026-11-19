// Copyright FNGameProj. All Rights Reserved.
// AthenaAIController — AI controller for bot players in BR.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "AthenaAIController.generated.h"

class UAthenaAIPerceptionComponent;
class UBehaviorTreeComponent;
class UBlackboardComponent;

UENUM(BlueprintType)
enum class EAIBehavior : uint8
{
	Idle          UMETA(DisplayName = "Idle"),
	LootSearching UMETA(DisplayName = "Loot Searching"),
	Engaging      UMETA(DisplayName = "Engaging Enemy"),
	Retreating    UMETA(DisplayName = "Retreating"),
	MovingToZone  UMETA(DisplayName = "Moving To Safe Zone"),
	Building      UMETA(DisplayName = "Building"),
};

UCLASS(BlueprintType, Blueprintable)
class FORTNITEGAME_API AAthenaAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAthenaAIController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// ── Perception ────────────────────────────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "AI|Perception")
	AActor* GetCurrentTarget() const { return CurrentTarget.Get(); }

	UFUNCTION(BlueprintCallable, Category = "AI|Perception")
	bool HasValidTarget() const { return CurrentTarget.IsValid(); }

	// ── Behavior control ──────────────────────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	void SetAIBehavior(EAIBehavior NewBehavior);

	UFUNCTION(BlueprintCallable, Category = "AI|Movement")
	void MoveToSafeZone();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	UAthenaAIPerceptionComponent* FortPerceptionComponent;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	TWeakObjectPtr<AActor> CurrentTarget;

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

private:
	void UpdateBlackboardValues();
};
