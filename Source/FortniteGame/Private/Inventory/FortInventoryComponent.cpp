// Copyright FNGameProj. All Rights Reserved.

#include "Inventory/FortInventoryComponent.h"
#include "Net/UnrealNetwork.h"

UFortInventoryComponent::UFortInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
	Slots.SetNum(FORT_INVENTORY_SLOTS);
}

void UFortInventoryComponent::BeginPlay() { Super::BeginPlay(); }

void UFortInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFortInventoryComponent, Slots);
	DOREPLIFETIME(UFortInventoryComponent, ActiveSlotIndex);
	DOREPLIFETIME(UFortInventoryComponent, Resources);
	DOREPLIFETIME(UFortInventoryComponent, Ammo);
}

bool UFortInventoryComponent::TryPickup(UFortWorldItem* WorldItem)
{
	if (!WorldItem) { return false; }

	// Try to stack on existing slot first
	int32 ExistingIdx = FindSlotWithItem(WorldItem->ItemDefinition);
	if (ExistingIdx != INDEX_NONE && WorldItem->Category == EFortItemCategory::Consumable)
	{
		Slots[ExistingIdx].Count += WorldItem->Count;
		BroadcastSlotChange(ExistingIdx);
		return true;
	}

	// Find empty slot
	int32 EmptyIdx = FindEmptySlot();
	if (EmptyIdx == INDEX_NONE) { return false; } // Inventory full

	Slots[EmptyIdx].ItemDefinition = WorldItem->ItemDefinition;
	Slots[EmptyIdx].Category       = WorldItem->Category;
	Slots[EmptyIdx].Count          = WorldItem->Count;
	Slots[EmptyIdx].AmmoInMag      = 0;
	BroadcastSlotChange(EmptyIdx);
	return true;
}

void UFortInventoryComponent::DropActiveSlot(FVector DropLocation, FVector ForwardDir)
{
	DropSlot(ActiveSlotIndex, DropLocation, ForwardDir);
}

void UFortInventoryComponent::DropSlot(int32 SlotIndex, FVector DropLocation, FVector ForwardDir)
{
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].IsEmpty()) { return; }

	// Spawn world pickup actor at location — Blueprint handles the actor spawn
	Slots[SlotIndex].Clear();
	BroadcastSlotChange(SlotIndex);
}

void UFortInventoryComponent::SelectSlot(int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex)) { return; }
	ActiveSlotIndex = SlotIndex;
	OnActiveSlotChanged.Broadcast(ActiveSlotIndex);
}

void UFortInventoryComponent::CycleSlot(int32 Direction)
{
	int32 Next = (ActiveSlotIndex + Direction + FORT_INVENTORY_SLOTS) % FORT_INVENTORY_SLOTS;
	SelectSlot(Next);
}

FFortInventorySlot UFortInventoryComponent::GetSlot(int32 Index) const
{
	if (Slots.IsValidIndex(Index)) { return Slots[Index]; }
	return FFortInventorySlot{};
}

bool UFortInventoryComponent::IsSlotEmpty(int32 Index) const
{
	return !Slots.IsValidIndex(Index) || Slots[Index].IsEmpty();
}

void UFortInventoryComponent::AddResource(EFortBuildMaterial Material, int32 Amount)
{
	int32 Cap = MaxWood;
	if (Material == EFortBuildMaterial::Stone) Cap = MaxStone;
	if (Material == EFortBuildMaterial::Metal) Cap = MaxMetal;

	int32 Current = Resources.GetAmount(Material);
	int32 Actual  = FMath::Min(Amount, Cap - Current);
	if (Actual <= 0) { return; }

	Resources.AddAmount(Material, Actual);
	OnResourceChanged.Broadcast(Material, Resources.GetAmount(Material));
}

bool UFortInventoryComponent::ConsumeResource(EFortBuildMaterial Material, int32 Amount)
{
	if (Resources.GetAmount(Material) < Amount) { return false; }
	Resources.AddAmount(Material, -Amount);
	OnResourceChanged.Broadcast(Material, Resources.GetAmount(Material));
	return true;
}

int32 UFortInventoryComponent::GetResourceAmount(EFortBuildMaterial Material) const
{
	return Resources.GetAmount(Material);
}

int32 UFortInventoryComponent::FindEmptySlot() const
{
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].IsEmpty()) { return i; }
	}
	return INDEX_NONE;
}

int32 UFortInventoryComponent::FindSlotWithItem(TSoftObjectPtr<UPrimaryDataAsset> ItemDef) const
{
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].ItemDefinition == ItemDef) { return i; }
	}
	return INDEX_NONE;
}

void UFortInventoryComponent::BroadcastSlotChange(int32 SlotIndex)
{
	if (Slots.IsValidIndex(SlotIndex))
	{
		OnSlotChanged.Broadcast(SlotIndex, Slots[SlotIndex]);
	}
}

void UFortInventoryComponent::OnRep_Slots()
{
	for (int32 i = 0; i < Slots.Num(); ++i) { BroadcastSlotChange(i); }
}

void UFortInventoryComponent::OnRep_ActiveSlotIndex()
{
	OnActiveSlotChanged.Broadcast(ActiveSlotIndex);
}
