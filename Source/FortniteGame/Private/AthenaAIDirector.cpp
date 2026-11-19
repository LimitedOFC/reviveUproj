// Copyright FNGameProj. All Rights Reserved.
#include "AthenaAIDirector.h"
#include "AthenaCharacter.h"
#include "AthenaAIController.h"
#include "Engine/World.h"

AAthenaAIDirector::AAthenaAIDirector()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAthenaAIDirector::BeginPlay()
{
	Super::BeginPlay();
}

void AAthenaAIDirector::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// Remove dead bots from tracking
	ActiveBots.RemoveAll([](AAthenaCharacter* Bot)
	{
		return !IsValid(Bot) || !Bot->IsAlive();
	});
}

void AAthenaAIDirector::SpawnBot(FVector Location)
{
	if (!BotCharacterClass || GetLivingBotCount() >= MaxBots) { return; }

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AAthenaCharacter* Bot = GetWorld()->SpawnActor<AAthenaCharacter>(BotCharacterClass, Location, FRotator::ZeroRotator, Params);
	if (Bot)
	{
		ActiveBots.Add(Bot);
		if (BotControllerClass)
		{
			AAthenaAIController* BotAI = GetWorld()->SpawnActor<AAthenaAIController>(BotControllerClass);
			if (BotAI) { BotAI->Possess(Bot); }
		}
	}
}

void AAthenaAIDirector::DespawnAllBots()
{
	for (AAthenaCharacter* Bot : ActiveBots)
	{
		if (IsValid(Bot)) { Bot->Destroy(); }
	}
	ActiveBots.Empty();
}

int32 AAthenaAIDirector::GetLivingBotCount() const
{
	int32 Count = 0;
	for (AAthenaCharacter* Bot : ActiveBots)
	{
		if (IsValid(Bot) && Bot->IsAlive()) { ++Count; }
	}
	return Count;
}
