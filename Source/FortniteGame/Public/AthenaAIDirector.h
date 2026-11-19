// Copyright FNGameProj. All Rights Reserved.
// AthenaAIDirector — manages bot population, spawning, and difficulty scaling.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AthenaAIDirector.generated.h"

class AAthenaAIController;
class AAthenaCharacter;

UCLASS(BlueprintType, Blueprintable, meta = (ShortTooltip = "Manages AI bot lifecycle in BR match"))
class FORTNITEGAME_API AAthenaAIDirector : public AActor
{
	GENERATED_BODY()

public:
	AAthenaAIDirector();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Director")
	int32 MaxBots = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Director")
	TSubclassOf<AAthenaCharacter> BotCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Director")
	TSubclassOf<AAthenaAIController> BotControllerClass;

	UFUNCTION(BlueprintCallable, Category = "AI|Director")
	void SpawnBot(FVector Location);

	UFUNCTION(BlueprintCallable, Category = "AI|Director")
	void DespawnAllBots();

	UFUNCTION(BlueprintPure, Category = "AI|Director")
	int32 GetLivingBotCount() const;

protected:
	UPROPERTY(Transient)
	TArray<AAthenaCharacter*> ActiveBots;
};
