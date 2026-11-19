// Copyright FNGameProj. All Rights Reserved.
// FortWeaponItemDefinition — data asset describing a weapon's stats and visuals.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "FortWeaponItemDefinition.generated.h"

UENUM(BlueprintType)
enum class EFortAmmoType : uint8
{
	LightBullets  UMETA(DisplayName = "Light Bullets"),
	MediumBullets UMETA(DisplayName = "Medium Bullets"),
	HeavyBullets  UMETA(DisplayName = "Heavy Bullets"),
	Shells        UMETA(DisplayName = "Shells"),
	Rockets       UMETA(DisplayName = "Rockets"),
	Arrows        UMETA(DisplayName = "Arrows"),
	InfiniteAmmo  UMETA(DisplayName = "Infinite (harvesting)"),
};

UCLASS(BlueprintType, Blueprintable, meta = (ShortTooltip = "Weapon item definition (data asset)"))
class FORTNITEGAME_API UFortWeaponItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ── Identity ──────────────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FName WeaponID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FGameplayTag WeaponTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FGameplayTag RarityTag;

	// ── Stats ─────────────────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float DamagePerShot = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float HeadshotMultiplier = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float FireRate = 5.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 MagazineSize = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float ReloadTime = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float BulletSpread = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float ADSSpread = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float BulletSpeed = 36000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float EffectiveRange = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float StructureDamage = 33.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	bool bIsAutomatic = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	bool bHasADS = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	EFortAmmoType AmmoType = EFortAmmoType::MediumBullets;

	/** How many pellets per shot (shotguns: 10, single: 1). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 PelletsPerShot = 1;

	// ── Visuals & Audio ───────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TSoftObjectPtr<class USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TSoftObjectPtr<class UTexture2D> PickupIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TSoftObjectPtr<class UTexture2D> LargePreviewImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TSoftObjectPtr<class USoundCue> FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TSoftObjectPtr<class USoundCue> ReloadSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TSoftObjectPtr<class USoundCue> EmptyMagSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TSoftObjectPtr<class UParticleSystem> MuzzleFlashFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TSoftObjectPtr<class UParticleSystem> ImpactFX;

	// ── UPrimaryDataAsset ─────────────────────────────────────────────────────
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("WeaponItemDefinition", GetFName());
	}
};
