// Copyright FNGameProj. All Rights Reserved.
// FortInventoryComponent — 5-slot BR inventory management.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/FortInventoryTypes.h"
#include "FortInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotChanged, int32, SlotIndex, FFortInventorySlot, NewSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActiveSlotChanged, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResourceChanged, EFortBuildMaterial, Material, int32, NewAmount);

static constexpr int32 FORT_INVENTORY_SLOTS = 5;

UCLASS(ClassGroup = Fort, BlueprintType, meta = (BlueprintSpawnableComponent))
class FORTNITEGAME_API UFortInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFortInventoryComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ── Slot management ───────────────────────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool TryPickup(UFortWorldItem* WorldItem);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropActiveSlot(FVector DropLocation, FVector ForwardDir);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropSlot(int32 SlotIndex, FVector DropLocation, FVector ForwardDir);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SelectSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CycleSlot(int32 Direction);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FFortInventorySlot GetSlot(int32 Index) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsSlotEmpty(int32 Index) const;

	// ── Resources ─────────────────────────────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "Inventory|Resources")
	void AddResource(EFortBuildMaterial Material, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Resources")
	bool ConsumeResource(EFortBuildMaterial Material, int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Inventory|Resources")
	int32 GetResourceAmount(EFortBuildMaterial Material) const;

	// ── Events ────────────────────────────────────────────────────────────────

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnSlotChanged OnSlotChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnActiveSlotChanged OnActiveSlotChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnResourceChanged OnResourceChanged;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Slots, BlueprintReadOnly, Category = "Inventory")
	TArray<FFortInventorySlot> Slots;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveSlotIndex, BlueprintReadOnly, Category = "Inventory")
	int32 ActiveSlotIndex = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory|Resources")
	FFortBuildingResources Resources;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory|Resources")
	FFortAmmoStash Ammo;

	/** Season 9 per-material resource caps. */
	static constexpr int32 MaxWood  = 999;
	static constexpr int32 MaxStone = 999;
	static constexpr int32 MaxMetal = 999;

	UFUNCTION() void OnRep_Slots();
	UFUNCTION() void OnRep_ActiveSlotIndex();

	int32 FindEmptySlot() const;
	int32 FindSlotWithItem(TSoftObjectPtr<UPrimaryDataAsset> ItemDef) const;
	void BroadcastSlotChange(int32 SlotIndex);
};
