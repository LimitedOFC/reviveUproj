// Copyright FNGameProj. All Rights Reserved.
// FortBuildingSettings — per-player building and edit preferences.
//
// Stored in GameUserSettings.ini under [/Script/FortniteGame.FortBuildingSettings].
// Every field maps 1-to-1 to a UI toggle or slider in the Game Settings → Building tab.
// All changes take effect immediately via ApplyToComponent().

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FortBuildingSettings.generated.h"

class UFortBuildingComponent;

// ─────────────────────────────────────────────────────────────────────────────
// Edit-mode behavior enum
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Controls how the edit cursor is confirmed and what happens on release.
 *
 *   Standard        — Press G to begin edit, click to confirm, right-click / G again to reset.
 *   EditOnRelease   — Edit begins on press; the shape confirmed when the key is released.
 *   BuilderPro      — Full Builder Pro layout: separate edit and confirm binds,
 *                     hold threshold before edit opens, release-to-confirm optional.
 */
UENUM(BlueprintType)
enum class EFortEditMode : uint8
{
	Standard      UMETA(DisplayName = "Standard"),
	EditOnRelease UMETA(DisplayName = "Edit On Release"),
	BuilderPro    UMETA(DisplayName = "Builder Pro"),
};

// ─────────────────────────────────────────────────────────────────────────────
// Settings object
// ─────────────────────────────────────────────────────────────────────────────

/**
 * UFortBuildingSettings
 *
 * Persisted per-user building preferences. Registered as a DeveloperSettings
 * category so they appear in Project Settings → Plugins → Building (editor)
 * and are written to GameUserSettings.ini at runtime via SaveConfig().
 *
 * Access at runtime:
 *   const UFortBuildingSettings* S = GetDefault<UFortBuildingSettings>();
 *   or via the static helper:
 *   UFortBuildingSettings::Get()
 */
UCLASS(config = GameUserSettings, defaultconfig,
	   meta = (DisplayName = "Building Settings"))
class FORTNITEGAME_API UFortBuildingSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UFortBuildingSettings();

	// ── Static accessor ───────────────────────────────────────────────────────

	/** Returns the mutable CDO so code can call SaveConfig() after changing values. */
	static UFortBuildingSettings* Get()
	{
		return GetMutableDefault<UFortBuildingSettings>();
	}

	// ── Simple Edit ───────────────────────────────────────────────────────────

	/**
	 * Simple Edit — when enabled:
	 *   • Clicking any cell immediately confirms the edit with no extra keypress.
	 *   • The edit cursor opens the moment the edit key is pressed (no hold threshold).
	 *   • Equivalent to ticking "Confirm Edit On Release" + "Edit On Release" together
	 *     in a single convenience toggle.
	 * Saved to config. Applied instantly.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite,
		Category = "Building|Simple Edit",
		meta = (DisplayName = "Simple Edit",
		        ToolTip = "One-click edit: open and confirm in a single input. Saves to config and applies immediately."))
	bool bSimpleEdit = false;

	// ── Edit-on-release family ─────────────────────────────────────────────────

	/**
	 * Edit On Release — the edit shape is confirmed automatically when the edit
	 * key is released instead of requiring a separate confirm click.
	 * Ignored when Simple Edit is active (Simple Edit implies this behaviour).
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite,
		Category = "Building|Edit Behaviour",
		meta = (DisplayName = "Edit On Release",
		        ToolTip = "Automatically confirm the edit when the edit key is released."))
	bool bEditOnRelease = false;

	/**
	 * Confirm Edit On Release — identical to Edit On Release but only applies
	 * during the confirmation step (useful for Builder Pro where open and confirm
	 * are on separate binds).
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite,
		Category = "Building|Edit Behaviour",
		meta = (DisplayName = "Confirm Edit On Release",
		        ToolTip = "Confirm the current edit shape when the confirm key is released."))
	bool bConfirmEditOnRelease = false;

	/**
	 * Reset On Release — if the player releases the edit key without selecting
	 * any cell the piece is reset to its default shape rather than left unchanged.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite,
		Category = "Building|Edit Behaviour",
		meta = (DisplayName = "Reset On Release",
		        ToolTip = "Reset the piece to its full shape when the edit key is released with no cell selected."))
	bool bResetOnRelease = false;

	/**
	 * Disable Pre-Edits — prevents the ghost preview from snapping to the
	 * last-used edit shape before the player begins a new edit interaction.
	 * Keeps every edit starting from the full (un-edited) piece shape.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite,
		Category = "Building|Edit Behaviour",
		meta = (DisplayName = "Disable Pre-Edits",
		        ToolTip = "Always start from the full piece shape; never carry over a previous edit shape."))
	bool bDisablePreEdits = false;

	// ── Turbobuild ────────────────────────────────────────────────────────────

	/**
	 * Turbobuild — holding the fire button places pieces as fast as the engine
	 * allows without requiring a separate click per piece.
	 * This matches the Season 9 Turbobuild setting exactly.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite,
		Category = "Building|Placement",
		meta = (DisplayName = "Turbobuild",
		        ToolTip = "Hold fire to continuously place pieces at maximum speed."))
	bool bTurbobuild = true;

	// ── Control-scheme support flags ──────────────────────────────────────────

	/**
	 * Builder Pro Support — enables the Builder Pro control scheme:
	 *   • Each build piece is mapped to a separate button.
	 *   • Edit open and edit confirm are on separate binds.
	 *   • Hold-to-open edit is active when this is enabled.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite,
		Category = "Building|Control Scheme",
		meta = (DisplayName = "Builder Pro Support",
		        ToolTip = "Activate the Builder Pro input layout (separate piece buttons, hold-to-edit)."))
	bool bBuilderProSupport = false;

	/**
	 * Simple Edit Support — allow Simple Edit to be toggled at runtime from the
	 * HUD or keybind. If false, Simple Edit can only be changed in Settings.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite,
		Category = "Building|Control Scheme",
		meta = (DisplayName = "Simple Edit Support",
		        ToolTip = "Allow Simple Edit to be toggled via a runtime keybind."))
	bool bSimpleEditSupport = true;

	// ── Instant Edit Toggle ───────────────────────────────────────────────────

	/**
	 * Instant Edit Toggle — pressing the edit key switches directly into edit
	 * mode on the aimed piece without any visual transition delay.
	 * When false a brief aim-and-open animation plays (cosmetic only).
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite,
		Category = "Building|Edit Behaviour",
		meta = (DisplayName = "Instant Edit Toggle",
		        ToolTip = "Skip the open-edit animation and enter edit mode instantly."))
	bool bInstantEditToggle = true;

	// ── Edit Hold Time ────────────────────────────────────────────────────────

	/**
	 * Edit Hold Time — seconds the player must hold the edit key before the
	 * edit cursor opens. Only active when Builder Pro Support is enabled.
	 * Range: 0.0 (open immediately) → 0.5 s.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite,
		Category = "Building|Timings",
		meta = (DisplayName = "Edit Hold Time (s)",
		        ClampMin = "0.0", ClampMax = "0.5", UIMin = "0.0", UIMax = "0.5",
		        ToolTip = "How long the edit key must be held before the edit cursor opens (Builder Pro only)."))
	float EditHoldTime = 0.0f;

	// ── Edit Reset Delay ──────────────────────────────────────────────────────

	/**
	 * Edit Reset Delay — seconds after a confirmed edit before the piece is
	 * eligible to be reset-on-release. Prevents accidental instant resets.
	 * Range: 0.0 (no delay) → 1.0 s.
	 */
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite,
		Category = "Building|Timings",
		meta = (DisplayName = "Edit Reset Delay (s)",
		        ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0",
		        ToolTip = "Minimum time after an edit is confirmed before reset-on-release can fire."))
	float EditResetDelay = 0.15f;

	// ── Helpers ───────────────────────────────────────────────────────────────

	/**
	 * Returns true if the effective behaviour should confirm on release.
	 * Simple Edit implies confirm-on-release, so either flag being set counts.
	 */
	UFUNCTION(BlueprintPure, Category = "Building|Settings")
	bool ShouldConfirmOnRelease() const
	{
		return bSimpleEdit || bConfirmEditOnRelease || bEditOnRelease;
	}

	/**
	 * Returns the effective hold time before edit opens.
	 * When Simple Edit or Instant Edit Toggle are active the hold time is 0.
	 */
	UFUNCTION(BlueprintPure, Category = "Building|Settings")
	float GetEffectiveEditHoldTime() const
	{
		if (bSimpleEdit || bInstantEditToggle) { return 0.0f; }
		if (!bBuilderProSupport)              { return 0.0f; }
		return EditHoldTime;
	}

	/**
	 * Push all settings immediately to a building component without restarting play.
	 * Called by the Settings UI after any value change.
	 */
	UFUNCTION(BlueprintCallable, Category = "Building|Settings")
	void ApplyToComponent(UFortBuildingComponent* Component) const;

	/**
	 * Saves current values to GameUserSettings.ini immediately.
	 * Always call this after modifying any property at runtime.
	 */
	UFUNCTION(BlueprintCallable, Category = "Building|Settings")
	void SaveSettings() { SaveConfig(); }

	// ── UDeveloperSettings ────────────────────────────────────────────────────

	virtual FName GetCategoryName() const override { return FName("Game"); }
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
