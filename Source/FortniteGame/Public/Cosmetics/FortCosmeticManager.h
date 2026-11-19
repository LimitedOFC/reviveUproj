// Copyright FNGameProj. All Rights Reserved.
// FortCosmeticManager — subsystem that owns the cosmetic registry and applies
// loadouts to character instances at runtime.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Cosmetics/FortCosmeticTypes.h"
#include "FortCosmeticManager.generated.h"

class ASkeletalMeshActor;
class UFortCosmeticPreviewActor;

/**
 * UFortCosmeticManager
 *
 * Game-instance subsystem responsible for:
 *   1. Caching the cosmetic registry from DT_CosmeticRegistry.
 *   2. Applying a FFortPlayerLoadout to a character's skeletal mesh components.
 *   3. Providing async-load helpers so cosmetics are streamed in without hitches.
 *   4. Tracking which cosmetics are "backported" vs native S9.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Fort Cosmetic Manager"))
class FORTNITEGAME_API UFortCosmeticManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	// ── USubsystem interface ──────────────────────────────────────────────────
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ── Registry access ───────────────────────────────────────────────────────

	/**
	 * Returns the registry row for a given CosmeticID, or nullptr if not found.
	 * Preferred over direct DataTable access; logs a warning on miss.
	 */
	const FCosmeticRegistryRow* FindCosmeticRow(FName CosmeticID) const;

	/**
	 * Returns every registry row that matches the given cosmetic slot tag.
	 * Useful for populating the locker grid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Cosmetics|Registry")
	TArray<FCosmeticRegistryRow> GetAllCosmeticsForSlot(FGameplayTag SlotTag) const;

	/**
	 * Returns every registry row flagged as backported (bIsBackported = true).
	 */
	UFUNCTION(BlueprintCallable, Category = "Cosmetics|Backport")
	TArray<FCosmeticRegistryRow> GetAllBackportedCosmetics() const;

	// ── Loadout application ───────────────────────────────────────────────────

	/**
	 * Applies the given loadout to TargetMeshComponent immediately.
	 * Synchronous — only call after assets are confirmed resident in memory.
	 */
	UFUNCTION(BlueprintCallable, Category = "Cosmetics|Apply")
	void ApplyOutfitToMesh(USkeletalMeshComponent* TargetMeshComponent, FName OutfitID);

	/**
	 * Attaches the back bling from LoadoutBackBlingID to the character's
	 * BackBlingSocket. Creates or reuses an existing attached component.
	 */
	UFUNCTION(BlueprintCallable, Category = "Cosmetics|Apply")
	void ApplyBackBling(AActor* OwnerActor, FName BackBlingID);

	/**
	 * Applies a wrap material instance to every weapon/vehicle component
	 * on OwnerActor that accepts wraps.
	 */
	UFUNCTION(BlueprintCallable, Category = "Cosmetics|Apply")
	void ApplyWrap(AActor* OwnerActor, FName WrapID);

	/**
	 * Begins async streaming of all assets referenced by the loadout.
	 * OnComplete fires when every asset is resident. Safe to call before
	 * the character actor is fully spawned.
	 */
	void AsyncLoadLoadout(const FFortPlayerLoadout& Loadout,
	                      FSimpleDelegate OnComplete);

	// ── Backport helpers ──────────────────────────────────────────────────────

	/**
	 * Returns the backport mapping row for the given CosmeticID, or nullptr.
	 * Used by editor tooling and the preview actor to show backport metadata.
	 */
	const FBackportMappingRow* FindBackportMapping(FName CosmeticID) const;

	/**
	 * Returns true if the cosmetic needs retargeting (BackportState = Pending).
	 */
	UFUNCTION(BlueprintCallable, Category = "Cosmetics|Backport")
	bool IsCosmeticBackportPending(FName CosmeticID) const;

	// ── DataTable references (set in DefaultGame.ini or via editor) ───────────

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetics|Data")
	TSoftObjectPtr<UDataTable> CosmeticRegistryTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetics|Data")
	TSoftObjectPtr<UDataTable> BackportMappingTable;

private:

	/** Loaded (hard-ref) pointer to the registry table, populated in Initialize. */
	UPROPERTY(Transient)
	UDataTable* LoadedRegistryTable;

	UPROPERTY(Transient)
	UDataTable* LoadedBackportTable;

	/** Active async load handle so we can cancel if the game shuts down. */
	TSharedPtr<struct FStreamableHandle> PendingLoadHandle;
};
