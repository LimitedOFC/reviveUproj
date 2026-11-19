// Copyright FNGameProj. All Rights Reserved.

#include "AthenaGameMode.h"
#include "AthenaCharacter.h"
#include "AthenaAIDirector.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"

AAthenaGameMode::AAthenaGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	DefaultPawnClass = AAthenaCharacter::StaticClass();
}

void AAthenaGameMode::BeginPlay()
{
	Super::BeginPlay();
	SpawnAIDirector();
	TransitionToPhase(EAthenaMatchPhase::WaitingForPlayers);

	GetWorldTimerManager().SetTimer(WaitTimer, this,
		&AAthenaGameMode::StartMatch,
		WaitForPlayersDuration, false);
}

void AAthenaGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AAthenaGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (AAthenaCharacter* Char = Cast<AAthenaCharacter>(NewPlayer->GetPawn()))
	{
		LivingPlayers.Add(Char);
		Char->OnPlayerEliminated.AddDynamic(this, &AAthenaGameMode::HandlePlayerElimination);
	}
}

void AAthenaGameMode::Logout(AController* Exiting)
{
	if (APlayerController* PC = Cast<APlayerController>(Exiting))
	{
		if (AAthenaCharacter* Char = Cast<AAthenaCharacter>(PC->GetPawn()))
		{
			LivingPlayers.Remove(Char);
		}
	}
	Super::Logout(Exiting);
}

void AAthenaGameMode::StartMatch()
{
	GetWorldTimerManager().ClearTimer(WaitTimer);
	TransitionToPhase(EAthenaMatchPhase::BattleBus);

	GetWorldTimerManager().SetTimer(BattleBusTimer, this,
		&AAthenaGameMode::OnBattleBusEnded,
		BattleBusDuration, false);
}

void AAthenaGameMode::OnBattleBusEnded()
{
	TransitionToPhase(EAthenaMatchPhase::Playing);
}

void AAthenaGameMode::HandlePlayerElimination(AAthenaCharacter* Eliminated, AAthenaCharacter* Eliminator)
{
	LivingPlayers.Remove(Eliminated);
	++TotalEliminations;

	const int32 Remaining = GetLivingPlayerCount();

	if (Remaining <= 10 && CurrentPhase == EAthenaMatchPhase::Playing)
	{
		TransitionToPhase(EAthenaMatchPhase::TopTen);
	}

	if (Remaining <= 1)
	{
		AAthenaCharacter* Winner = LivingPlayers.Num() > 0 ? LivingPlayers[0] : nullptr;
		EndMatch(Winner);
	}
}

void AAthenaGameMode::EndMatch(AAthenaCharacter* Winner)
{
	TransitionToPhase(EAthenaMatchPhase::Victory);

	GetWorldTimerManager().SetTimer(BattleBusTimer, [this]()
	{
		TransitionToPhase(EAthenaMatchPhase::PostMatch);
	}, 10.0f, false);
}

int32 AAthenaGameMode::GetLivingPlayerCount() const
{
	int32 Count = 0;
	for (AAthenaCharacter* P : LivingPlayers)
	{
		if (IsValid(P) && P->IsAlive()) { ++Count; }
	}
	return Count;
}

void AAthenaGameMode::TransitionToPhase(EAthenaMatchPhase NewPhase)
{
	CurrentPhase = NewPhase;
	// Blueprint event hook: OnPhaseChanged (implemented in BP_AthenaGameMode)
}

void AAthenaGameMode::SpawnAIDirector()
{
	if (!AIDirectorClass) { return; }
	AIDirector = GetWorld()->SpawnActor<AAthenaAIDirector>(AIDirectorClass);
}
