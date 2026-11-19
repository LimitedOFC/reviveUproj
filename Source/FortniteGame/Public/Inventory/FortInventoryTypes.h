// Copyright FNGameProj. All Rights Reserved.
// FortInventoryTypes — shared inventory structs, enums and forward declarations.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FortInventoryTypes.generated.h"

class UFortWeaponItemDefinition;
class UFortConsumableItemDefinition;

/** Item category — determines which inventory slot rules apply. */
UENUM(BlueprintType)
enum class EFortItemCategory : uint8
{
	Weapon      UMETA(DisplayName = "Weapon"),
	Consumable  UMETA(DisplayName = "Consumable"),
	Resource    UMETA(DisplayName = "Resource"),
	Ammo        UMETA(DisplayName = "Ammo"),
	Buildable   UMETA(DisplayName = "Buildable"),
	Quest       UMETA(DisplayName = "Quest"),
};

/** Build material type. */
UENUM(BlueprintType)
enum class EFortBuildMaterial : uint8
{
	Wood   UMETA(DisplayName = "Wood"),
	Stone  UMETA(DisplayName = "Stone"),
	Metal  UMETA(DisplayName = "Metal"),
};

/** Represents one slot in the 5-slot BR inventory. */
USTRUCT(BlueprintType)
struct FORTNITEGAME_API FFortInventorySlot
{
	GENERATED_BODY()

	/** Primary data asset path — either a weapon or consumable def. */
	UPROPERTY(BlueprintReadWrite, Category = "Slot")
	TSoftObjectPtr<UPrimaryDataAsset> ItemDefinition;

	UPROPERTY(BlueprintReadWrite, Category = "Slot")
	EFortItemCategory Category = EFortItemCategory::Weapon;

	/** Stack count — 1 for weapons, variable for consumables and ammo. */
	UPROPERTY(BlueprintReadWrite, Category = "Slot")
	int32 Count = 0;

	/** Current ammo in mag (weapons only). */
	UPROPERTY(BlueprintReadWrite, Category = "Slot")
	int32 AmmoInMag = 0;

	bool IsEmpty() const { return Count <= 0; }
	void Clear() { ItemDefinition.Reset(); Count = 0; AmmoInMag = 0; Category = EFortItemCategory::Weapon; }
};

/** Resource stacks: wood, stone, metal. */
USTRUCT(BlueprintType)
struct FORTNITEGAME_API FFortBuildingResources
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Resources")
	int32 Wood = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Resources")
	int32 Stone = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Resources")
	int32 Metal = 0;

	int32 GetAmount(EFortBuildMaterial Mat) const
	{
		switch (Mat)
		{
		case EFortBuildMaterial::Wood:  return Wood;
		case EFortBuildMaterial::Stone: return Stone;
		case EFortBuildMaterial::Metal: return Metal;
		default: return 0;
		}
	}

	void AddAmount(EFortBuildMaterial Mat, int32 Delta)
	{
		switch (Mat)
		{
		case EFortBuildMaterial::Wood:  Wood  = FMath::Max(Wood  + Delta, 0); break;
		case EFortBuildMaterial::Stone: Stone = FMath::Max(Stone + Delta, 0); break;
		case EFortBuildMaterial::Metal: Metal = FMath::Max(Metal + Delta, 0); break;
		}
	}
};

/** Ammo stash. */
USTRUCT(BlueprintType)
struct FORTNITEGAME_API FFortAmmoStash
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Ammo")
	int32 LightBullets = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Ammo")
	int32 MediumBullets = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Ammo")
	int32 HeavyBullets = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Ammo")
	int32 Shells = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Ammo")
	int32 Rockets = 0;
};

/** A world pickup item — the base class for pickups spawned on the ground. */
UCLASS(BlueprintType, Blueprintable)
class FORTNITEGAME_API UFortWorldItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Item")
	TSoftObjectPtr<UPrimaryDataAsset> ItemDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Item")
	int32 Count = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Item")
	EFortItemCategory Category = EFortItemCategory::Weapon;
};
