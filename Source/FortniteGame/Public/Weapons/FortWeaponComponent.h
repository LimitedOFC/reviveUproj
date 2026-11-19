// Copyright FNGameProj. All Rights Reserved.
// FortWeaponComponent — actor component that handles firing, reloading, ADS.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/FortWeaponItemDefinition.h"
#include "FortWeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, CurrentAmmo, int32, MagSize);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFired);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponReloaded);

UCLASS(ClassGroup = Fort, BlueprintType, meta = (BlueprintSpawnableComponent))
class FORTNITEGAME_API UFortWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFortWeaponComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ── API ───────────────────────────────────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeaponDef(UFortWeaponItemDefinition* InDef);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetADS(bool bAiming);

	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool IsReloading() const { return bIsReloading; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool IsADS() const { return bIsADS; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	UFortWeaponItemDefinition* GetCurrentDef() const { return CurrentWeaponDef; }

	// ── Events ────────────────────────────────────────────────────────────────

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnAmmoChanged OnAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponFired OnWeaponFired;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponReloaded OnWeaponReloaded;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmo, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentAmmo = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
	bool bIsReloading = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
	bool bIsADS = false;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	UFortWeaponItemDefinition* CurrentWeaponDef;

	FTimerHandle ReloadTimerHandle;
	FTimerHandle FireRateTimerHandle;
	bool bIsFiring = false;

	void ExecuteShot();
	void FinishReload();
	void DoFullAutoFire();

	UFUNCTION()
	void OnRep_CurrentAmmo();
};
