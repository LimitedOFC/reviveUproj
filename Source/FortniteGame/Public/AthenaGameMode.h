// Copyright FNGameProj. All Rights Reserved.
// AthenaGameMode — Chapter 1 Season 9 BR game mode (100-player match).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AthenaGameMode.generated.h"

class AAthenaCharacter;
class AAthenaAIDirector;
class UFortStormSystem;

UENUM(BlueprintType)
enum class EAthenaMatchPhase : uint8
{
	WaitingForPlayers UMETA(DisplayName = "Waiting For Players"),
	BattleBus         UMETA(DisplayName = "Battle Bus"),
	Playing           UMETA(DisplayName = "Playing"),
	TopTen            UMETA(DisplayName = "Top 10"),
	Victory           UMETA(DisplayName = "Victory Royale"),
	PostMatch         UMETA(DisplayName = "Post Match"),
};

UCLASS(BlueprintType, Blueprintable)
class FORTNITEGAME_API AAthenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAthenaGameMode(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	// ── Match flow ────────────────────────────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "Match")
	void StartMatch();

	UFUNCTION(BlueprintCallable, Category = "Match")
	void EndMatch(AAthenaCharacter* Winner);

	UFUNCTION(BlueprintCallable, Category = "Match")
	void HandlePlayerElimination(AAthenaCharacter* Eliminated, AAthenaCharacter* Eliminator);

	UFUNCTION(BlueprintPure, Category = "Match")
	EAthenaMatchPhase GetCurrentPhase() const { return CurrentPhase; }

	UFUNCTION(BlueprintPure, Category = "Match")
	int32 GetLivingPlayerCount() const;

	UFUNCTION(BlueprintPure, Category = "Match")
	int32 GetTotalEliminationCount() const { return TotalEliminations; }

	// ── Config ────────────────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match|Config")
	int32 MaxPlayers = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match|Config")
	float BattleBusDuration = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match|Config")
	float WaitForPlayersDuration = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match|Config")
	TSubclassOf<AAthenaAIDirector> AIDirectorClass;

protected:
	UPROPERTY(Transient)
	TArray<AAthenaCharacter*> LivingPlayers;

	UPROPERTY(Transient)
	AAthenaAIDirector* AIDirector;

	UPROPERTY(BlueprintReadOnly, Category = "Match")
	EAthenaMatchPhase CurrentPhase = EAthenaMatchPhase::WaitingForPlayers;

	int32 TotalEliminations = 0;

	FTimerHandle BattleBusTimer;
	FTimerHandle WaitTimer;

	void TransitionToPhase(EAthenaMatchPhase NewPhase);
	void OnBattleBusEnded();
	void SpawnAIDirector();
};
