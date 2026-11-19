// Copyright FNGameProj. All Rights Reserved.
// FortBuildingComponent — Chapter 1 Season 9 building, editing, and all advanced
// build-settings behaviours (Simple Edit, Edit-on-Release, Turbobuild, etc.)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/FortInventoryTypes.h"
#include "Building/FortBuildingSettings.h"
#include "FortBuildingComponent.generated.h"

// ─────────────────────────────────────────────────────────────────────────────
// Enums
// ─────────────────────────────────────────────────────────────────────────────

UENUM(BlueprintType)
enum class EFortBuildPieceType : uint8
{
	Wall  UMETA(DisplayName = "Wall"),
	Floor UMETA(DisplayName = "Floor"),
	Ramp  UMETA(DisplayName = "Ramp"),
	Roof  UMETA(DisplayName = "Roof / Pyramid"),
};

UENUM(BlueprintType)
enum class EFortEditPreset : uint8
{
	None       UMETA(DisplayName = "Full (no edit)"),
	Window     UMETA(DisplayName = "Window"),
	Door       UMETA(DisplayName = "Door"),
	HalfWall   UMETA(DisplayName = "Half Wall"),
	CornerCut  UMETA(DisplayName = "Corner Cut / Arch"),
	DoubleArch UMETA(DisplayName = "Double Arch"),
};

/** Internal state machine for the edit interaction. */
UENUM(BlueprintType)
enum class EFortEditState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Holding     UMETA(DisplayName = "Holding (waiting for hold threshold)"),
	Open        UMETA(DisplayName = "Edit cursor open"),
	Confirming  UMETA(DisplayName = "Confirming (waiting for release)"),
};

// ─────────────────────────────────────────────────────────────────────────────
// Build cost helper
// ─────────────────────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FORTNITEGAME_API FFortBuildCost
{
	GENERATED_BODY()

	static int32 Get(EFortBuildPieceType /*Piece*/, EFortBuildMaterial /*Mat*/)
	{
		return 10; // Uniform 10 per piece — Season 9 cost
	}
};

// ─────────────────────────────────────────────────────────────────────────────
// Delegates
// ─────────────────────────────────────────────────────────────────────────────

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuildModeChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEditModeChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEditConfirmed,  EFortEditPreset, AppliedPreset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEditReset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPiecePlaced,    EFortBuildPieceType, PlacedPiece);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettingsApplied, const UFortBuildingSettings*, NewSettings);

// ─────────────────────────────────────────────────────────────────────────────
// Component
// ─────────────────────────────────────────────────────────────────────────────

UCLASS(ClassGroup = Fort, BlueprintType, meta = (BlueprintSpawnableComponent))
class FORTNITEGAME_API UFortBuildingComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFortBuildingComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// ─────────────────────────────────────────────────────────────────────────
	// Settings application
	// ─────────────────────────────────────────────────────────────────────────

	/**
	 * Push a full UFortBuildingSettings snapshot into this component.
	 * Called automatically on BeginPlay (from the CDO) and whenever the
	 * player changes any setting at runtime.
	 */
	UFUNCTION(BlueprintCallable, Category = "Building|Settings")
	void ApplyBuildingSettings(const UFortBuildingSettings* Settings);

	/** Re-reads the CDO and applies. Convenience wrapper for Blueprint. */
	UFUNCTION(BlueprintCallable, Category = "Building|Settings")
	void RefreshSettingsFromConfig();

	// ─────────────────────────────────────────────────────────────────────────
	// Build mode
	// ─────────────────────────────────────────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetBuildModeActive(bool bActive);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetActivePieceType(EFortBuildPieceType PieceType);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetActiveMaterial(EFortBuildMaterial Material);

	/** Attempt to place the active piece. Returns false if not enough resources. */
	UFUNCTION(BlueprintCallable, Category = "Building")
	bool TryPlacePiece();

	/** Rotates the placement ghost 90° clockwise. */
	UFUNCTION(BlueprintCallable, Category = "Building")
	void RotatePlacement();

	// ─────────────────────────────────────────────────────────────────────────
	// Edit mode — input entry points
	// These are called by the player input component on press / release.
	// The component handles all state-machine logic internally.
	// ─────────────────────────────────────────────────────────────────────────

	/**
	 * Called when the Edit key is PRESSED.
	 *   • Simple Edit / Instant Edit Toggle → opens edit immediately.
	 *   • Builder Pro with hold threshold    → starts the hold timer.
	 *   • Standard                           → opens edit immediately.
	 */
	UFUNCTION(BlueprintCallable, Category = "Building|Edit")
	void EditKeyPressed();

	/**
	 * Called when the Edit key is RELEASED.
	 *   • Edit On Release / Simple Edit      → confirms (or resets) depending on cell selection.
	 *   • Reset On Release with no selection → resets.
	 *   • Builder Pro hold not met           → cancels without opening.
	 */
	UFUNCTION(BlueprintCallable, Category = "Building|Edit")
	void EditKeyReleased();

	/**
	 * Called when the Confirm Edit key is PRESSED (Builder Pro layout only).
	 * In Standard / Simple Edit this is the same bind as EditKeyPressed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Building|Edit")
	void ConfirmEditKeyPressed();

	/**
	 * Called when the Confirm Edit key is RELEASED.
	 * Honours bConfirmEditOnRelease.
	 */
	UFUNCTION(BlueprintCallable, Category = "Building|Edit")
	void ConfirmEditKeyReleased();

	// ─────────────────────────────────────────────────────────────────────────
	// Edit mode — programmatic API (called by above and by Blueprint)
	// ─────────────────────────────────────────────────────────────────────────

	/** Opens the edit cursor on the aimed piece. */
	UFUNCTION(BlueprintCallable, Category = "Building|Edit")
	void BeginEdit();

	/**
	 * Confirms the currently-selected edit shape.
	 * When bSimpleEdit is active this also closes edit mode.
	 */
	UFUNCTION(BlueprintCallable, Category = "Building|Edit")
	void ConfirmEdit();

	/**
	 * Resets the aimed piece to its full (un-edited) shape and closes edit mode.
	 */
	UFUNCTION(BlueprintCallable, Category = "Building|Edit")
	void ResetEdit();

	/** Applies a named preset shape without requiring cell-by-cell selection. */
	UFUNCTION(BlueprintCallable, Category = "Building|Edit")
	void ApplyEditPreset(EFortEditPreset Preset);

	// ─────────────────────────────────────────────────────────────────────────
	// Turbobuild
	// ─────────────────────────────────────────────────────────────────────────

	/**
	 * Called when the fire button is held in build mode.
	 * If Turbobuild is enabled, starts auto-placing pieces every frame.
	 */
	UFUNCTION(BlueprintCallable, Category = "Building|Turbobuild")
	void StartTurbobuild();

	/** Stops Turbobuild auto-place loop. */
	UFUNCTION(BlueprintCallable, Category = "Building|Turbobuild")
	void StopTurbobuild();

	// ─────────────────────────────────────────────────────────────────────────
	// Accessors
	// ─────────────────────────────────────────────────────────────────────────

	UFUNCTION(BlueprintPure, Category = "Building")
	bool IsBuildModeActive() const { return bBuildModeActive; }

	UFUNCTION(BlueprintPure, Category = "Building")
	bool IsEditModeActive() const { return EditState != EFortEditState::Idle; }

	UFUNCTION(BlueprintPure, Category = "Building")
	EFortEditState GetEditState() const { return EditState; }

	UFUNCTION(BlueprintPure, Category = "Building")
	EFortBuildPieceType GetActivePiece() const { return ActivePieceType; }

	UFUNCTION(BlueprintPure, Category = "Building")
	EFortBuildMaterial GetActiveMaterial() const { return ActiveMaterial; }

	UFUNCTION(BlueprintPure, Category = "Building")
	EFortEditPreset GetPendingEditPreset() const { return PendingPreset; }

	UFUNCTION(BlueprintPure, Category = "Building")
	bool IsTurbobuilding() const { return bTurbobuilding; }

	/** Returns the last confirmed edit preset (for pre-edit preview). */
	UFUNCTION(BlueprintPure, Category = "Building|Edit")
	EFortEditPreset GetLastConfirmedPreset() const { return LastConfirmedPreset; }

	// ─────────────────────────────────────────────────────────────────────────
	// Config
	// ─────────────────────────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building|Config")
	float PlacementTraceLength = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building|Config")
	float BuildGridSize = 256.0f;

	/** Minimum seconds between Turbobuild placements (caps fire rate). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building|Config",
		meta = (ClampMin = "0.01", ClampMax = "1.0"))
	float TurbobuildInterval = 0.05f;

	// ─────────────────────────────────────────────────────────────────────────
	// Events — bind in Blueprint to drive HUD, animations, SFX
	// ─────────────────────────────────────────────────────────────────────────

	UPROPERTY(BlueprintAssignable, Category = "Building|Events")
	FOnBuildModeChanged OnBuildModeEnabled;

	UPROPERTY(BlueprintAssignable, Category = "Building|Events")
	FOnBuildModeChanged OnBuildModeDisabled;

	UPROPERTY(BlueprintAssignable, Category = "Building|Events")
	FOnEditModeChanged OnEditOpened;

	UPROPERTY(BlueprintAssignable, Category = "Building|Events")
	FOnEditConfirmed OnEditConfirmed;

	UPROPERTY(BlueprintAssignable, Category = "Building|Events")
	FOnEditReset OnEditResetFired;

	UPROPERTY(BlueprintAssignable, Category = "Building|Events")
	FOnPiecePlaced OnPiecePlaced;

	UPROPERTY(BlueprintAssignable, Category = "Building|Events")
	FOnSettingsApplied OnSettingsApplied;

protected:

	// ─────────────────────────────────────────────────────────────────────────
	// Runtime state
	// ─────────────────────────────────────────────────────────────────────────

	bool                bBuildModeActive   = false;
	bool                bTurbobuilding     = false;
	EFortBuildPieceType ActivePieceType    = EFortBuildPieceType::Wall;
	EFortBuildMaterial  ActiveMaterial     = EFortBuildMaterial::Wood;
	float               CurrentRotationYaw = 0.0f;

	// Edit state machine
	EFortEditState EditState          = EFortEditState::Idle;
	EFortEditPreset PendingPreset     = EFortEditPreset::None;
	EFortEditPreset LastConfirmedPreset = EFortEditPreset::None;

	// Copied from settings on ApplyBuildingSettings — avoids CDO lookups per-frame
	bool  bOpt_SimpleEdit           = false;
	bool  bOpt_EditOnRelease        = false;
	bool  bOpt_ConfirmEditOnRelease = false;
	bool  bOpt_ResetOnRelease       = false;
	bool  bOpt_DisablePreEdits      = false;
	bool  bOpt_Turbobuild           = true;
	bool  bOpt_BuilderProSupport    = false;
	bool  bOpt_InstantEditToggle    = true;
	float fOpt_EditHoldTime         = 0.0f;
	float fOpt_EditResetDelay       = 0.15f;

	// Timers
	FTimerHandle EditHoldTimerHandle;
	FTimerHandle ResetDelayTimerHandle;
	FTimerHandle TurbobuildTimerHandle;

	// Time tracking for reset delay
	float LastConfirmTime = 0.0f;

	// ─────────────────────────────────────────────────────────────────────────
	// Internal helpers
	// ─────────────────────────────────────────────────────────────────────────

	void OpenEditCursor();
	void CloseEditCursor();
	void ExecuteConfirm();
	void ExecuteReset();
	bool HasCellSelected() const;

	/** True when enough time has passed since last confirm to allow reset-on-release. */
	bool ResetDelayElapsed() const;

	void OnEditHoldThresholdMet();
	void DoTurbobuildTick();

	FVector GetSnappedPlacementLocation() const;
	AActor* FindBuildTarget() const;
};
