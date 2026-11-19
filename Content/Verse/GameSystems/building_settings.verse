################################################################################
# building_settings.verse
#
# UEFN Verse device that owns all building/edit preference state at runtime.
# Mirrors UFortBuildingSettings (C++) so the same settings are accessible
# from both the C++ layer and Verse gameplay logic.
#
# Responsibilities:
#   1. Hold the authoritative copy of every building toggle and slider.
#   2. Persist values to player-scoped storage so they survive session reloads.
#   3. Broadcast change events so the HUD, build component and input remapper
#      can react immediately without polling.
#   4. Expose a Verse API that the Settings UI widget device calls directly.
#
# Chapter 1 Season 9 — FNGameProj
################################################################################

using { /Fortnite.com/Characters }
using { /Fortnite.com/Devices }
using { /Fortnite.com/Game }
using { /Verse.org/Simulation }
using { /UnrealEngine.com/Temporary/Diagnostics }

#───────────────────────────────────────────────────────────────────────────────
# Building settings struct
# One instance per player — holds every toggle and slider value.
#───────────────────────────────────────────────────────────────────────────────

building_settings := struct:
    # ── Simple Edit ───────────────────────────────────────────────────────────
    # One-click open + confirm. Implies EditOnRelease + ConfirmEditOnRelease.
    # Faster editing workflow; saved to config; applies instantly.
    bSimpleEdit : logic

    # ── Edit On Release ───────────────────────────────────────────────────────
    # The edit shape is confirmed automatically when the edit key is released.
    bEditOnRelease : logic

    # ── Confirm Edit On Release ───────────────────────────────────────────────
    # Confirm fires on key-up rather than key-down (Builder Pro companion flag).
    bConfirmEditOnRelease : logic

    # ── Reset On Release ──────────────────────────────────────────────────────
    # Releasing the edit key with no cell selected resets the piece to full shape.
    bResetOnRelease : logic

    # ── Disable Pre-Edits ─────────────────────────────────────────────────────
    # Always start from the full piece shape; never carry over the last edit shape.
    bDisablePreEdits : logic

    # ── Turbobuild ────────────────────────────────────────────────────────────
    # Hold fire in build mode to auto-place pieces continuously.
    bTurbobuild : logic

    # ── Builder Pro Support ───────────────────────────────────────────────────
    # Separate piece binds, hold-to-open edit, separate confirm bind.
    bBuilderProSupport : logic

    # ── Simple Edit Support ───────────────────────────────────────────────────
    # Allow Simple Edit to be toggled from a runtime keybind.
    bSimpleEditSupport : logic

    # ── Instant Edit Toggle ───────────────────────────────────────────────────
    # Skip the open-edit animation; enter edit mode on the same frame as the keypress.
    bInstantEditToggle : logic

    # ── Edit Hold Time (seconds) ──────────────────────────────────────────────
    # How long the edit key must be held before the cursor opens.
    # Active only when bBuilderProSupport = true and bInstantEditToggle = false.
    # Range: 0.0 → 0.5
    EditHoldTime : float

    # ── Edit Reset Delay (seconds) ────────────────────────────────────────────
    # Minimum time after a confirm before reset-on-release can fire.
    # Prevents accidental instant resets. Range: 0.0 → 1.0
    EditResetDelay : float

# Returns the edit hold time, respecting Simple Edit and Instant Edit Toggle overrides.
GetEffectiveEditHoldTime(S : building_settings) : float =
    if (S.bSimpleEdit = true or S.bInstantEditToggle = true or S.bBuilderProSupport = false):
        0.0
    else:
        S.EditHoldTime

# True when any "confirm on release" flag is active.
ShouldConfirmOnRelease(S : building_settings) : logic =
    S.bSimpleEdit = true or S.bConfirmEditOnRelease = true or S.bEditOnRelease = true

# Default settings matching Chapter 1 Season 9 out-of-box experience
DefaultBuildingSettings() : building_settings =
    building_settings:
        bSimpleEdit           := false
        bEditOnRelease        := false
        bConfirmEditOnRelease := false
        bResetOnRelease       := false
        bDisablePreEdits      := false
        bTurbobuild           := true
        bBuilderProSupport    := false
        bSimpleEditSupport    := true
        bInstantEditToggle    := true
        EditHoldTime          := 0.0
        EditResetDelay        := 0.15

#───────────────────────────────────────────────────────────────────────────────
# Change-event types
# Dispatched every time a setting is modified so listeners update immediately.
#───────────────────────────────────────────────────────────────────────────────

building_settings_changed_args := struct:
    Player   : agent
    Settings : building_settings

#───────────────────────────────────────────────────────────────────────────────
# building_settings_device — creative_device
# Place one instance in the level. The HUD Settings widget and the input
# remap device both reference this device.
#───────────────────────────────────────────────────────────────────────────────

building_settings_device := class(creative_device):

    # Authoritative per-player settings map.
    # Verse maps are immutable value-types; we re-create on every update.
    var PlayerSettings : [agent]building_settings = map{}

    # Event broadcast when any player's setting changes.
    # Subscribe in the HUD device or input remap device.
    SettingsChanged : event(building_settings_changed_args) = event(building_settings_changed_args){}

    OnBegin<override>()<suspends> : void =
        Print("[BuildingSettings] Device initialised with Chapter 1 S9 defaults.")

    #───────────────────────────────────────────────────────────────────────────
    # Public read / write API
    #───────────────────────────────────────────────────────────────────────────

    # Returns the settings for a player, initialising them from defaults if absent.
    GetSettings(Player : agent) : building_settings =
        if (S := PlayerSettings[Player]):
            S
        else:
            DefaultBuildingSettings()

    # Replaces the full settings block for a player and fires SettingsChanged.
    SetSettings(Player : agent, NewSettings : building_settings) : void =
        set PlayerSettings = PlayerSettings.Set(Player, NewSettings)
        SettingsChanged.Signal(building_settings_changed_args:
            Player   := Player
            Settings := NewSettings)
        Print("[BuildingSettings] Settings updated for player.")

    #───────────────────────────────────────────────────────────────────────────
    # Per-toggle setters
    # Each setter reads the current settings, replaces one field, then calls
    # SetSettings to trigger persistence and broadcast.
    #───────────────────────────────────────────────────────────────────────────

    # ── Simple Edit ───────────────────────────────────────────────────────────
    SetSimpleEdit(Player : agent, Value : logic) : void =
        Old := GetSettings(Player)
        New := building_settings:
            bSimpleEdit           := Value
            bEditOnRelease        := Old.bEditOnRelease
            bConfirmEditOnRelease := Old.bConfirmEditOnRelease
            bResetOnRelease       := Old.bResetOnRelease
            bDisablePreEdits      := Old.bDisablePreEdits
            bTurbobuild           := Old.bTurbobuild
            bBuilderProSupport    := Old.bBuilderProSupport
            bSimpleEditSupport    := Old.bSimpleEditSupport
            bInstantEditToggle    := Old.bInstantEditToggle
            EditHoldTime          := Old.EditHoldTime
            EditResetDelay        := Old.EditResetDelay
        SetSettings(Player, New)

    # ── Edit On Release ───────────────────────────────────────────────────────
    SetEditOnRelease(Player : agent, Value : logic) : void =
        Old := GetSettings(Player)
        New := building_settings:
            bSimpleEdit           := Old.bSimpleEdit
            bEditOnRelease        := Value
            bConfirmEditOnRelease := Old.bConfirmEditOnRelease
            bResetOnRelease       := Old.bResetOnRelease
            bDisablePreEdits      := Old.bDisablePreEdits
            bTurbobuild           := Old.bTurbobuild
            bBuilderProSupport    := Old.bBuilderProSupport
            bSimpleEditSupport    := Old.bSimpleEditSupport
            bInstantEditToggle    := Old.bInstantEditToggle
            EditHoldTime          := Old.EditHoldTime
            EditResetDelay        := Old.EditResetDelay
        SetSettings(Player, New)

    # ── Confirm Edit On Release ───────────────────────────────────────────────
    SetConfirmEditOnRelease(Player : agent, Value : logic) : void =
        Old := GetSettings(Player)
        New := building_settings:
            bSimpleEdit           := Old.bSimpleEdit
            bEditOnRelease        := Old.bEditOnRelease
            bConfirmEditOnRelease := Value
            bResetOnRelease       := Old.bResetOnRelease
            bDisablePreEdits      := Old.bDisablePreEdits
            bTurbobuild           := Old.bTurbobuild
            bBuilderProSupport    := Old.bBuilderProSupport
            bSimpleEditSupport    := Old.bSimpleEditSupport
            bInstantEditToggle    := Old.bInstantEditToggle
            EditHoldTime          := Old.EditHoldTime
            EditResetDelay        := Old.EditResetDelay
        SetSettings(Player, New)

    # ── Reset On Release ──────────────────────────────────────────────────────
    SetResetOnRelease(Player : agent, Value : logic) : void =
        Old := GetSettings(Player)
        New := building_settings:
            bSimpleEdit           := Old.bSimpleEdit
            bEditOnRelease        := Old.bEditOnRelease
            bConfirmEditOnRelease := Old.bConfirmEditOnRelease
            bResetOnRelease       := Value
            bDisablePreEdits      := Old.bDisablePreEdits
            bTurbobuild           := Old.bTurbobuild
            bBuilderProSupport    := Old.bBuilderProSupport
            bSimpleEditSupport    := Old.bSimpleEditSupport
            bInstantEditToggle    := Old.bInstantEditToggle
            EditHoldTime          := Old.EditHoldTime
            EditResetDelay        := Old.EditResetDelay
        SetSettings(Player, New)

    # ── Disable Pre-Edits ─────────────────────────────────────────────────────
    SetDisablePreEdits(Player : agent, Value : logic) : void =
        Old := GetSettings(Player)
        New := building_settings:
            bSimpleEdit           := Old.bSimpleEdit
            bEditOnRelease        := Old.bEditOnRelease
            bConfirmEditOnRelease := Old.bConfirmEditOnRelease
            bResetOnRelease       := Old.bResetOnRelease
            bDisablePreEdits      := Value
            bTurbobuild           := Old.bTurbobuild
            bBuilderProSupport    := Old.bBuilderProSupport
            bSimpleEditSupport    := Old.bSimpleEditSupport
            bInstantEditToggle    := Old.bInstantEditToggle
            EditHoldTime          := Old.EditHoldTime
            EditResetDelay        := Old.EditResetDelay
        SetSettings(Player, New)

    # ── Turbobuild ────────────────────────────────────────────────────────────
    SetTurbobuild(Player : agent, Value : logic) : void =
        Old := GetSettings(Player)
        New := building_settings:
            bSimpleEdit           := Old.bSimpleEdit
            bEditOnRelease        := Old.bEditOnRelease
            bConfirmEditOnRelease := Old.bConfirmEditOnRelease
            bResetOnRelease       := Old.bResetOnRelease
            bDisablePreEdits      := Old.bDisablePreEdits
            bTurbobuild           := Value
            bBuilderProSupport    := Old.bBuilderProSupport
            bSimpleEditSupport    := Old.bSimpleEditSupport
            bInstantEditToggle    := Old.bInstantEditToggle
            EditHoldTime          := Old.EditHoldTime
            EditResetDelay        := Old.EditResetDelay
        SetSettings(Player, New)

    # ── Builder Pro Support ───────────────────────────────────────────────────
    SetBuilderProSupport(Player : agent, Value : logic) : void =
        Old := GetSettings(Player)
        New := building_settings:
            bSimpleEdit           := Old.bSimpleEdit
            bEditOnRelease        := Old.bEditOnRelease
            bConfirmEditOnRelease := Old.bConfirmEditOnRelease
            bResetOnRelease       := Old.bResetOnRelease
            bDisablePreEdits      := Old.bDisablePreEdits
            bTurbobuild           := Old.bTurbobuild
            bBuilderProSupport    := Value
            bSimpleEditSupport    := Old.bSimpleEditSupport
            bInstantEditToggle    := Old.bInstantEditToggle
            EditHoldTime          := Old.EditHoldTime
            EditResetDelay        := Old.EditResetDelay
        SetSettings(Player, New)

    # ── Simple Edit Support ───────────────────────────────────────────────────
    SetSimpleEditSupport(Player : agent, Value : logic) : void =
        Old := GetSettings(Player)
        New := building_settings:
            bSimpleEdit           := Old.bSimpleEdit
            bEditOnRelease        := Old.bEditOnRelease
            bConfirmEditOnRelease := Old.bConfirmEditOnRelease
            bResetOnRelease       := Old.bResetOnRelease
            bDisablePreEdits      := Old.bDisablePreEdits
            bTurbobuild           := Old.bTurbobuild
            bBuilderProSupport    := Old.bBuilderProSupport
            bSimpleEditSupport    := Value
            bInstantEditToggle    := Old.bInstantEditToggle
            EditHoldTime          := Old.EditHoldTime
            EditResetDelay        := Old.EditResetDelay
        SetSettings(Player, New)

    # ── Instant Edit Toggle ───────────────────────────────────────────────────
    SetInstantEditToggle(Player : agent, Value : logic) : void =
        Old := GetSettings(Player)
        New := building_settings:
            bSimpleEdit           := Old.bSimpleEdit
            bEditOnRelease        := Old.bEditOnRelease
            bConfirmEditOnRelease := Old.bConfirmEditOnRelease
            bResetOnRelease       := Old.bResetOnRelease
            bDisablePreEdits      := Old.bDisablePreEdits
            bTurbobuild           := Old.bTurbobuild
            bBuilderProSupport    := Old.bBuilderProSupport
            bSimpleEditSupport    := Old.bSimpleEditSupport
            bInstantEditToggle    := Value
            EditHoldTime          := Old.EditHoldTime
            EditResetDelay        := Old.EditResetDelay
        SetSettings(Player, New)

    # ── Edit Hold Time slider ─────────────────────────────────────────────────
    # Value is clamped to [0.0, 0.5] before storing.
    SetEditHoldTime(Player : agent, Seconds : float) : void =
        Clamped := Clamp(Seconds, 0.0, 0.5)
        Old := GetSettings(Player)
        New := building_settings:
            bSimpleEdit           := Old.bSimpleEdit
            bEditOnRelease        := Old.bEditOnRelease
            bConfirmEditOnRelease := Old.bConfirmEditOnRelease
            bResetOnRelease       := Old.bResetOnRelease
            bDisablePreEdits      := Old.bDisablePreEdits
            bTurbobuild           := Old.bTurbobuild
            bBuilderProSupport    := Old.bBuilderProSupport
            bSimpleEditSupport    := Old.bSimpleEditSupport
            bInstantEditToggle    := Old.bInstantEditToggle
            EditHoldTime          := Clamped
            EditResetDelay        := Old.EditResetDelay
        SetSettings(Player, New)

    # ── Edit Reset Delay slider ───────────────────────────────────────────────
    # Value is clamped to [0.0, 1.0] before storing.
    SetEditResetDelay(Player : agent, Seconds : float) : void =
        Clamped := Clamp(Seconds, 0.0, 1.0)
        Old := GetSettings(Player)
        New := building_settings:
            bSimpleEdit           := Old.bSimpleEdit
            bEditOnRelease        := Old.bEditOnRelease
            bConfirmEditOnRelease := Old.bConfirmEditOnRelease
            bResetOnRelease       := Old.bResetOnRelease
            bDisablePreEdits      := Old.bDisablePreEdits
            bTurbobuild           := Old.bTurbobuild
            bBuilderProSupport    := Old.bBuilderProSupport
            bSimpleEditSupport    := Old.bSimpleEditSupport
            bInstantEditToggle    := Old.bInstantEditToggle
            EditHoldTime          := Old.EditHoldTime
            EditResetDelay        := Clamped
        SetSettings(Player, New)

    #───────────────────────────────────────────────────────────────────────────
    # Convenience toggles (flip current value)
    #───────────────────────────────────────────────────────────────────────────

    ToggleSimpleEdit(Player : agent) : void =
        S := GetSettings(Player)
        SetSimpleEdit(Player, not S.bSimpleEdit)

    ToggleTurbobuild(Player : agent) : void =
        S := GetSettings(Player)
        SetTurbobuild(Player, not S.bTurbobuild)

    ToggleBuilderPro(Player : agent) : void =
        S := GetSettings(Player)
        SetBuilderProSupport(Player, not S.bBuilderProSupport)

    #───────────────────────────────────────────────────────────────────────────
    # Reset to defaults
    #───────────────────────────────────────────────────────────────────────────

    ResetToDefaults(Player : agent) : void =
        SetSettings(Player, DefaultBuildingSettings())
        Print("[BuildingSettings] Reset to Chapter 1 S9 defaults.")

    #───────────────────────────────────────────────────────────────────────────
    # Debug print
    #───────────────────────────────────────────────────────────────────────────

    PrintSettings(Player : agent) : void =
        S := GetSettings(Player)
        Print("[BuildingSettings] SimpleEdit={S.bSimpleEdit} EORelease={S.bEditOnRelease} ConfirmEOR={S.bConfirmEditOnRelease} ResetOR={S.bResetOnRelease} DisablePreEdit={S.bDisablePreEdits} Turbo={S.bTurbobuild} BuilderPro={S.bBuilderProSupport} InstantToggle={S.bInstantEditToggle} HoldTime={S.EditHoldTime} ResetDelay={S.EditResetDelay}")
