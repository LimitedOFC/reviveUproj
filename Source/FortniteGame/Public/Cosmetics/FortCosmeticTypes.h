// Copyright FNGameProj. All Rights Reserved.
// Cosmetic system data types — structs for DataTable rows and runtime state.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "FortCosmeticTypes.generated.h"

// ─────────────────────────────────────────────────────────────────────────────
// Enums
// ─────────────────────────────────────────────────────────────────────────────

/** High-level cosmetic slot categories. Mirrors Cosmetic.Slot.* gameplay tags. */
UENUM(BlueprintType)
enum class EFortCosmeticSlot : uint8
{
	Character       UMETA(DisplayName = "Outfit / Character"),
	BackBling       UMETA(DisplayName = "Back Bling"),
	Pickaxe         UMETA(DisplayName = "Harvesting Tool"),
	Glider          UMETA(DisplayName = "Glider"),
	Emote           UMETA(DisplayName = "Emote"),
	Spray           UMETA(DisplayName = "Spray"),
	LoadingScreen   UMETA(DisplayName = "Loading Screen"),
	Wrap            UMETA(DisplayName = "Wrap"),
	Contrail        UMETA(DisplayName = "Skydive Contrail"),
	Pet             UMETA(DisplayName = "Pet"),
	Toy             UMETA(DisplayName = "Toy"),
	MusicPack       UMETA(DisplayName = "Music Pack"),
	Banner          UMETA(DisplayName = "Banner"),
};

/** Backport pipeline state for a cosmetic item. */
UENUM(BlueprintType)
enum class EFortBackportState : uint8
{
	Native      UMETA(DisplayName = "Native S9 — no retarget needed"),
	Retargeted  UMETA(DisplayName = "Retargeted from later rig"),
	Pending     UMETA(DisplayName = "Imported but retarget not complete"),
};

/** Season-of-origin category used for filtering and pipeline selection. */
UENUM(BlueprintType)
enum class EFortCosmeticSourceSeason : uint8
{
	Chapter1_S1   UMETA(DisplayName = "Chapter 1 Season 1"),
	Chapter1_S2   UMETA(DisplayName = "Chapter 1 Season 2"),
	Chapter1_S3   UMETA(DisplayName = "Chapter 1 Season 3"),
	Chapter1_S4   UMETA(DisplayName = "Chapter 1 Season 4"),
	Chapter1_S5   UMETA(DisplayName = "Chapter 1 Season 5"),
	Chapter1_S6   UMETA(DisplayName = "Chapter 1 Season 6"),
	Chapter1_S7   UMETA(DisplayName = "Chapter 1 Season 7"),
	Chapter1_S8   UMETA(DisplayName = "Chapter 1 Season 8"),
	Chapter1_S9   UMETA(DisplayName = "Chapter 1 Season 9 (native)"),
	Chapter1_S10  UMETA(DisplayName = "Chapter 1 Season X"),
	Chapter2      UMETA(DisplayName = "Chapter 2"),
	Chapter3      UMETA(DisplayName = "Chapter 3"),
	Chapter4      UMETA(DisplayName = "Chapter 4"),
	Chapter5      UMETA(DisplayName = "Chapter 5"),
	Custom        UMETA(DisplayName = "Custom / fan-made"),
};

// ─────────────────────────────────────────────────────────────────────────────
// DataTable structs
// ─────────────────────────────────────────────────────────────────────────────

/**
 * FCosmeticRegistryRow
 * One row in DT_CosmeticRegistry — describes a single cosmetic item that can
 * be equipped on the S9 character rig.
 */
USTRUCT(BlueprintType)
struct FORTNITEGAME_API FCosmeticRegistryRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Globally unique identifier for this cosmetic (e.g. "Outfit_RamirezBase"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Identity")
	FName CosmeticID;

	/** Localised display name shown in the locker UI. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Identity")
	FText DisplayName;

	/** Short description shown in the locker detail panel. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Identity")
	FText Description;

	/** Gameplay tag indicating the cosmetic slot (Cosmetic.Slot.*). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Classification")
	FGameplayTag CosmeticSlotTag;

	/** Gameplay tag indicating rarity (Cosmetic.Rarity.*). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Classification")
	FGameplayTag RarityTag;

	/** Body type tag (Cosmetic.BodyType.*) — determines which base mesh is used. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Rig")
	FGameplayTag BodyTypeTag;

	/** Which season this cosmetic originally came from. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Backport")
	FGameplayTag SourceSeasonTag;

	/** Current state of the backport pipeline for this cosmetic. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Backport")
	FGameplayTag BackportStateTag;

	/** Soft reference to the Skeletal Mesh asset bound to S9_BaseSkeleton. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Assets")
	TSoftObjectPtr<class USkeletalMesh> SkeletalMesh;

	/**
	 * Ordered list of material instances to apply to the skeletal mesh,
	 * matching the mesh's material slot order.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Assets")
	TArray<TSoftObjectPtr<class UMaterialInstance>> MaterialSlots;

	/** Socket name on the character skeleton where a back bling attaches. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Rig")
	FName BackBlingSocketName = FName("BackBlingSocket");

	/** Preview rotation offset in the locker preview camera (degrees). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|UI")
	float PreviewRotation = 0.0f;

	/** Thumbnail texture shown in the locker grid. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|UI")
	TSoftObjectPtr<class UTexture2D> ThumbnailTexture;

	/** True if this is a fan-made / custom cosmetic (not from Epic). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Meta")
	bool bIsCustom = false;

	/** True if this cosmetic was originally authored for a later season and backported. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Backport")
	bool bIsBackported = false;

	/** Free-text note for the cosmetic author (not shown in-game). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cosmetic|Meta")
	FString AuthorNote;
};

/**
 * FBackportMappingRow
 * One row in DT_BackportMapping — tracks source → retargeted asset mapping
 * for each backported cosmetic, plus which IK retarget profile was used.
 */
USTRUCT(BlueprintType)
struct FORTNITEGAME_API FBackportMappingRow : public FTableRowBase
{
	GENERATED_BODY()

	/** CosmeticID this mapping applies to (matches DT_CosmeticRegistry). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Backport")
	FName SourceCosmeticID;

	/** Human-readable label for the source season (e.g. "Chapter2_S1"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Backport")
	FString SourceSeason;

	/** Soft ref to the original source mesh (may live in a separate import folder). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Backport")
	TSoftObjectPtr<class USkeletalMesh> SourceSkeletalMesh;

	/** Soft ref to the final S9-skeleton mesh output by the retarget step. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Backport")
	TSoftObjectPtr<class USkeletalMesh> TargetSkeletalMesh;

	/** IK Retarget asset used to produce TargetSkeletalMesh. Empty if native. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Backport")
	TSoftObjectPtr<UObject> RetargetProfile;

	/** Whether a retarget was needed (false for native S9 cosmetics). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Backport")
	bool bRetargetRequired = false;

	/** Whether material slots had to be remapped to M_CharacterBase instances. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Backport")
	bool bMaterialRemapRequired = false;

	/** Notes on what material remapping was done (for reference). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Backport")
	FString MaterialRemapNotes;

	/** Workflow status: "Pending", "Complete", "NeedsReview". */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Backport")
	FString BackportStatus = TEXT("Pending");

	/** Free-text notes for the person doing the backport. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Backport")
	FString Notes;
};

// ─────────────────────────────────────────────────────────────────────────────
// Runtime loadout struct
// ─────────────────────────────────────────────────────────────────────────────

/**
 * FFortPlayerLoadout
 * Holds the full active cosmetic selection for one player at runtime.
 * Stored on FortPlayerState and replicated to all clients.
 */
USTRUCT(BlueprintType)
struct FORTNITEGAME_API FFortPlayerLoadout
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Loadout")
	FName OutfitID;

	UPROPERTY(BlueprintReadWrite, Category = "Loadout")
	FName BackBlingID;

	UPROPERTY(BlueprintReadWrite, Category = "Loadout")
	FName PickaxeID;

	UPROPERTY(BlueprintReadWrite, Category = "Loadout")
	FName GliderID;

	UPROPERTY(BlueprintReadWrite, Category = "Loadout")
	FName ContrailID;

	UPROPERTY(BlueprintReadWrite, Category = "Loadout")
	FName WrapID;

	UPROPERTY(BlueprintReadWrite, Category = "Loadout")
	FName MusicPackID;

	UPROPERTY(BlueprintReadWrite, Category = "Loadout")
	FName BannerID;

	FFortPlayerLoadout()
		: OutfitID("Outfit_RamirezBase")
		, BackBlingID("BackBling_Default")
		, PickaxeID("Pickaxe_Default")
		, GliderID("Glider_Default")
		, ContrailID(NAME_None)
		, WrapID(NAME_None)
		, MusicPackID(NAME_None)
		, BannerID(NAME_None)
	{}
};
