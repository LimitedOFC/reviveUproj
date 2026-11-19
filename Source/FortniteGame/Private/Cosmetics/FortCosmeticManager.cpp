// Copyright FNGameProj. All Rights Reserved.

#include "Cosmetics/FortCosmeticManager.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/SkeletalMesh.h"
#include "Materials/MaterialInstance.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogFortCosmetics, Log, All);

// ─────────────────────────────────────────────────────────────────────────────
// USubsystem lifecycle
// ─────────────────────────────────────────────────────────────────────────────

void UFortCosmeticManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Hard-load the registry and backport tables at startup.
	// Both tables are small (~kilobytes) so synchronous load is acceptable.
	if (!CosmeticRegistryTable.IsNull())
	{
		LoadedRegistryTable = CosmeticRegistryTable.LoadSynchronous();
		if (!LoadedRegistryTable)
		{
			UE_LOG(LogFortCosmetics, Warning,
				TEXT("UFortCosmeticManager: Failed to load CosmeticRegistryTable at '%s'"),
				*CosmeticRegistryTable.ToString());
		}
		else
		{
			UE_LOG(LogFortCosmetics, Log,
				TEXT("UFortCosmeticManager: Loaded cosmetic registry with %d rows."),
				LoadedRegistryTable->GetRowMap().Num());
		}
	}

	if (!BackportMappingTable.IsNull())
	{
		LoadedBackportTable = BackportMappingTable.LoadSynchronous();
	}
}

void UFortCosmeticManager::Deinitialize()
{
	if (PendingLoadHandle.IsValid())
	{
		PendingLoadHandle->CancelHandle();
	}
	Super::Deinitialize();
}

// ─────────────────────────────────────────────────────────────────────────────
// Registry access
// ─────────────────────────────────────────────────────────────────────────────

const FCosmeticRegistryRow* UFortCosmeticManager::FindCosmeticRow(FName CosmeticID) const
{
	if (!LoadedRegistryTable)
	{
		UE_LOG(LogFortCosmetics, Warning,
			TEXT("FindCosmeticRow: Registry table not loaded."));
		return nullptr;
	}

	static const FString ContextStr(TEXT("UFortCosmeticManager::FindCosmeticRow"));
	const FCosmeticRegistryRow* Row =
		LoadedRegistryTable->FindRow<FCosmeticRegistryRow>(CosmeticID, ContextStr, /*bWarnIfMissing=*/true);

	return Row;
}

TArray<FCosmeticRegistryRow> UFortCosmeticManager::GetAllCosmeticsForSlot(FGameplayTag SlotTag) const
{
	TArray<FCosmeticRegistryRow> Result;
	if (!LoadedRegistryTable) { return Result; }

	TArray<FCosmeticRegistryRow*> AllRows;
	LoadedRegistryTable->GetAllRows<FCosmeticRegistryRow>(TEXT("GetAllCosmeticsForSlot"), AllRows);

	for (const FCosmeticRegistryRow* Row : AllRows)
	{
		if (Row && Row->CosmeticSlotTag == SlotTag)
		{
			Result.Add(*Row);
		}
	}
	return Result;
}

TArray<FCosmeticRegistryRow> UFortCosmeticManager::GetAllBackportedCosmetics() const
{
	TArray<FCosmeticRegistryRow> Result;
	if (!LoadedRegistryTable) { return Result; }

	TArray<FCosmeticRegistryRow*> AllRows;
	LoadedRegistryTable->GetAllRows<FCosmeticRegistryRow>(TEXT("GetAllBackportedCosmetics"), AllRows);

	for (const FCosmeticRegistryRow* Row : AllRows)
	{
		if (Row && Row->bIsBackported)
		{
			Result.Add(*Row);
		}
	}
	return Result;
}

// ─────────────────────────────────────────────────────────────────────────────
// Loadout application
// ─────────────────────────────────────────────────────────────────────────────

void UFortCosmeticManager::ApplyOutfitToMesh(USkeletalMeshComponent* TargetMeshComponent, FName OutfitID)
{
	if (!TargetMeshComponent)
	{
		UE_LOG(LogFortCosmetics, Warning, TEXT("ApplyOutfitToMesh: TargetMeshComponent is null."));
		return;
	}

	const FCosmeticRegistryRow* Row = FindCosmeticRow(OutfitID);
	if (!Row)
	{
		UE_LOG(LogFortCosmetics, Warning,
			TEXT("ApplyOutfitToMesh: Cosmetic '%s' not found in registry."), *OutfitID.ToString());
		return;
	}

	// Apply skeletal mesh
	USkeletalMesh* Mesh = Row->SkeletalMesh.LoadSynchronous();
	if (Mesh)
	{
		TargetMeshComponent->SetSkeletalMesh(Mesh);
	}

	// Apply material instances in slot order
	for (int32 i = 0; i < Row->MaterialSlots.Num(); ++i)
	{
		UMaterialInstance* Mat = Row->MaterialSlots[i].LoadSynchronous();
		if (Mat)
		{
			TargetMeshComponent->SetMaterial(i, Mat);
		}
	}

	UE_LOG(LogFortCosmetics, Log,
		TEXT("ApplyOutfitToMesh: Applied '%s' to %s."),
		*OutfitID.ToString(),
		*TargetMeshComponent->GetOwner()->GetName());
}

void UFortCosmeticManager::ApplyBackBling(AActor* OwnerActor, FName BackBlingID)
{
	if (!OwnerActor || BackBlingID == NAME_None) { return; }

	const FCosmeticRegistryRow* Row = FindCosmeticRow(BackBlingID);
	if (!Row || Row->SkeletalMesh.IsNull()) { return; }

	// Find or create a child skeletal mesh component on the socket
	USkeletalMeshComponent* BackBlingComp = nullptr;
	TArray<USkeletalMeshComponent*> ExistingComps;
	OwnerActor->GetComponents<USkeletalMeshComponent>(ExistingComps);

	for (USkeletalMeshComponent* Comp : ExistingComps)
	{
		if (Comp && Comp->GetFName() == FName("BackBlingMesh"))
		{
			BackBlingComp = Comp;
			break;
		}
	}

	if (!BackBlingComp)
	{
		BackBlingComp = NewObject<USkeletalMeshComponent>(OwnerActor, FName("BackBlingMesh"));
		BackBlingComp->RegisterComponent();
		BackBlingComp->AttachToComponent(
			OwnerActor->GetRootComponent(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			Row->BackBlingSocketName);
	}

	USkeletalMesh* Mesh = Row->SkeletalMesh.LoadSynchronous();
	if (Mesh)
	{
		BackBlingComp->SetSkeletalMesh(Mesh);
	}
}

void UFortCosmeticManager::ApplyWrap(AActor* OwnerActor, FName WrapID)
{
	if (!OwnerActor || WrapID == NAME_None) { return; }

	const FCosmeticRegistryRow* Row = FindCosmeticRow(WrapID);
	if (!Row || Row->MaterialSlots.Num() == 0) { return; }

	UMaterialInstance* WrapMat = Row->MaterialSlots[0].LoadSynchronous();
	if (!WrapMat) { return; }

	// Apply wrap to all static and skeletal mesh components that are tagged for wraps
	TArray<UMeshComponent*> MeshComps;
	OwnerActor->GetComponents<UMeshComponent>(MeshComps);
	for (UMeshComponent* MeshComp : MeshComps)
	{
		if (MeshComp && MeshComp->ComponentHasTag(FName("SupportsWrap")))
		{
			for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
			{
				MeshComp->SetMaterial(i, WrapMat);
			}
		}
	}
}

void UFortCosmeticManager::AsyncLoadLoadout(const FFortPlayerLoadout& Loadout,
                                             FSimpleDelegate OnComplete)
{
	TArray<FSoftObjectPath> PathsToLoad;

	auto AddIfValid = [&](FName ID)
	{
		const FCosmeticRegistryRow* Row = FindCosmeticRow(ID);
		if (!Row) { return; }
		if (!Row->SkeletalMesh.IsNull())   { PathsToLoad.Add(Row->SkeletalMesh.ToSoftObjectPath()); }
		if (!Row->ThumbnailTexture.IsNull()){ PathsToLoad.Add(Row->ThumbnailTexture.ToSoftObjectPath()); }
		for (const auto& MatSlot : Row->MaterialSlots)
		{
			if (!MatSlot.IsNull()) { PathsToLoad.Add(MatSlot.ToSoftObjectPath()); }
		}
	};

	AddIfValid(Loadout.OutfitID);
	AddIfValid(Loadout.BackBlingID);
	AddIfValid(Loadout.PickaxeID);
	AddIfValid(Loadout.GliderID);
	AddIfValid(Loadout.ContrailID);
	AddIfValid(Loadout.WrapID);

	FStreamableManager& StreamManager = UAssetManager::GetStreamableManager();
	PendingLoadHandle = StreamManager.RequestAsyncLoad(
		PathsToLoad,
		[OnComplete]() { OnComplete.ExecuteIfBound(); },
		FStreamableManager::AsyncLoadHighPriority);
}

// ─────────────────────────────────────────────────────────────────────────────
// Backport helpers
// ─────────────────────────────────────────────────────────────────────────────

const FBackportMappingRow* UFortCosmeticManager::FindBackportMapping(FName CosmeticID) const
{
	if (!LoadedBackportTable) { return nullptr; }
	static const FString ContextStr(TEXT("UFortCosmeticManager::FindBackportMapping"));
	return LoadedBackportTable->FindRow<FBackportMappingRow>(CosmeticID, ContextStr, false);
}

bool UFortCosmeticManager::IsCosmeticBackportPending(FName CosmeticID) const
{
	const FCosmeticRegistryRow* Row = FindCosmeticRow(CosmeticID);
	if (!Row) { return false; }

	static const FGameplayTag PendingTag =
		FGameplayTag::RequestGameplayTag(FName("Cosmetic.Backport.Pending"));
	return Row->BackportStateTag == PendingTag;
}
