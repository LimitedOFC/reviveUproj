// Copyright FNGameProj. All Rights Reserved.
// AthenaCharacter — the base player character class for Fortnite BR (Season 9).
// Drives movement, inventory integration, building, health/shield, cosmetics.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Inventory/FortInventoryTypes.h"
#include "Cosmetics/FortCosmeticTypes.h"
#include "AthenaCharacter.generated.h"

class UAbilitySystemComponent;
class UFortAttributeSet;
class UFortInventoryComponent;
class UFortBuildingComponent;
class UFortWeaponComponent;
class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class SWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChanged, float, NewShield, float, MaxShield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerEliminated, AAthenaCharacter*, Eliminated, AAthenaCharacter*, Eliminator);

/** Movement modes specific to Athena BR. */
UENUM(BlueprintType)
enum class EAthenaMovementMode : uint8
{
	Walking         UMETA(DisplayName = "Walking"),
	Sprinting       UMETA(DisplayName = "Sprinting"),
	Crouching       UMETA(DisplayName = "Crouching"),
	Skydiving       UMETA(DisplayName = "Skydiving"),
	Gliding         UMETA(DisplayName = "Gliding"),
	Swimming        UMETA(DisplayName = "Swimming"),
	InVehicle       UMETA(DisplayName = "In Vehicle"),
	Downed          UMETA(DisplayName = "Downed"),
};

UCLASS(BlueprintType, Blueprintable, meta = (ShortTooltip = "Fortnite BR player character"))
class FORTNITEGAME_API AAthenaCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAthenaCharacter(const FObjectInitializer& ObjectInitializer);

	// ── IAbilitySystemInterface ───────────────────────────────────────────────
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// ── ACharacter overrides ──────────────────────────────────────────────────
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	                          AController* EventInstigator, AActor* DamageCauser) override;
	UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// ── Movement ──────────────────────────────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "Athena|Movement")
	void StartSprint();

	UFUNCTION(BlueprintCallable, Category = "Athena|Movement")
	void StopSprint();

	UFUNCTION(BlueprintCallable, Category = "Athena|Movement")
	void StartCrouch();

	UFUNCTION(BlueprintCallable, Category = "Athena|Movement")
	void StopCrouch();

	UFUNCTION(BlueprintCallable, Category = "Athena|Movement")
	void OpenGlider();

	UFUNCTION(BlueprintCallable, Category = "Athena|Movement")
	void StartSkydive();

	UFUNCTION(BlueprintPure, Category = "Athena|Movement")
	EAthenaMovementMode GetAthenaMovementMode() const { return CurrentMovementMode; }

	// ── Health & Shield ───────────────────────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "Athena|Health")
	void ApplyHealthChange(float Delta, AActor* DamageSource);

	UFUNCTION(BlueprintCallable, Category = "Athena|Health")
	void ApplyShieldChange(float Delta, AActor* DamageSource);

	UFUNCTION(BlueprintPure, Category = "Athena|Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Athena|Health")
	float GetCurrentShield() const { return CurrentShield; }

	UFUNCTION(BlueprintPure, Category = "Athena|Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Athena|Health")
	float GetMaxShield() const { return MaxShield; }

	UFUNCTION(BlueprintPure, Category = "Athena|Health")
	bool IsAlive() const { return CurrentHealth > 0.0f; }

	UFUNCTION(BlueprintPure, Category = "Athena|Health")
	bool IsDowned() const { return bIsDowned; }

	// ── Weapon & Inventory ────────────────────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "Athena|Weapon")
	void EquipWeapon(UFortWeaponItemDefinition* WeaponDef);

	UFUNCTION(BlueprintCallable, Category = "Athena|Weapon")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Athena|Weapon")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Athena|Weapon")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Athena|Weapon")
	void ADS(bool bAiming);

	UFUNCTION(BlueprintCallable, Category = "Athena|Inventory")
	void PickupItem(UFortWorldItem* WorldItem);

	UFUNCTION(BlueprintCallable, Category = "Athena|Inventory")
	void DropCurrentItem();

	UFUNCTION(BlueprintCallable, Category = "Athena|Inventory")
	void CycleInventorySlot(int32 Direction);

	UFUNCTION(BlueprintCallable, Category = "Athena|Inventory")
	void SelectInventorySlot(int32 SlotIndex);

	// ── Building ──────────────────────────────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "Athena|Building")
	void ToggleBuildMode();

	UFUNCTION(BlueprintCallable, Category = "Athena|Building")
	void PlaceBuildPiece();

	UFUNCTION(BlueprintCallable, Category = "Athena|Building")
	void StartEditBuilding();

	UFUNCTION(BlueprintCallable, Category = "Athena|Building")
	void ConfirmEdit();

	UFUNCTION(BlueprintCallable, Category = "Athena|Building")
	void ResetEdit();

	UFUNCTION(BlueprintCallable, Category = "Athena|Settings")
	void ToggleBuildingSettings();

	UFUNCTION(BlueprintPure, Category = "Athena|Building")
	bool IsInBuildMode() const { return bIsInBuildMode; }

	// ── Cosmetics ─────────────────────────────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "Athena|Cosmetics")
	void ApplyLoadout(const FFortPlayerLoadout& NewLoadout);

	UFUNCTION(BlueprintPure, Category = "Athena|Cosmetics")
	const FFortPlayerLoadout& GetCurrentLoadout() const { return CurrentLoadout; }

	// ── Events ────────────────────────────────────────────────────────────────

	UPROPERTY(BlueprintAssignable, Category = "Athena|Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Athena|Events")
	FOnShieldChanged OnShieldChanged;

	UPROPERTY(BlueprintAssignable, Category = "Athena|Events")
	FOnPlayerEliminated OnPlayerEliminated;

protected:

	// ── Components ────────────────────────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UFortInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UFortBuildingComponent* BuildingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UFortWeaponComponent* WeaponComponent;

	/** Secondary skeletal mesh for back bling attachment. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BackBlingMesh;

	// ── Replicated state ──────────────────────────────────────────────────────

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, BlueprintReadOnly, Category = "Athena|Health")
	float CurrentHealth = 100.0f;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentShield, BlueprintReadOnly, Category = "Athena|Health")
	float CurrentShield = 0.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Athena|Health")
	float MaxHealth = 100.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Athena|Health")
	float MaxShield = 100.0f;

	UPROPERTY(ReplicatedUsing = OnRep_bIsDowned, BlueprintReadOnly, Category = "Athena|Health")
	bool bIsDowned = false;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentLoadout, BlueprintReadOnly, Category = "Athena|Cosmetics")
	FFortPlayerLoadout CurrentLoadout;

	// ── Local-only state ──────────────────────────────────────────────────────

	UPROPERTY(BlueprintReadOnly, Category = "Athena|Movement")
	EAthenaMovementMode CurrentMovementMode = EAthenaMovementMode::Walking;

	UPROPERTY(BlueprintReadOnly, Category = "Athena|Building")
	bool bIsInBuildMode = false;

	UPROPERTY(BlueprintReadOnly, Category = "Athena|Movement")
	bool bIsSprinting = false;

	// ── Rep notifies ──────────────────────────────────────────────────────────

	UFUNCTION()
	void OnRep_CurrentHealth();

	UFUNCTION()
	void OnRep_CurrentShield();

	UFUNCTION()
	void OnRep_bIsDowned();

	UFUNCTION()
	void OnRep_CurrentLoadout();

	// ── Internal helpers ──────────────────────────────────────────────────────

	void HandleElimination(AActor* Eliminator);
	void UpdateMovementSpeed();

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void ScrollInventory(float Value);
	void StopEditBuilding();
	void ConfirmEditPressed();
	void ConfirmEditReleased();

	TSharedPtr<SWidget> BuildingSettingsMenu;

	/** Season 9 movement constants. */
	static constexpr float S9_SprintSpeed  = 725.0f;
	static constexpr float S9_WalkSpeed    = 400.0f;
	static constexpr float S9_CrouchSpeed  = 200.0f;
	static constexpr float S9_JumpVelocity = 855.0f;
};
