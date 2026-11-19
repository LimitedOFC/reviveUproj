################################################################################
# player_loadout.verse
#
# Manages the per-player cosmetic loadout in UEFN.
# Stores the active selection for each slot and broadcasts changes so the
# character actor (C++ side) can apply the new mesh/material.
#
# Chapter 1 Season 9 — FNGameProj
################################################################################

using { /Fortnite.com/Characters }
using { /Fortnite.com/Devices }
using { /Fortnite.com/Game }
using { /Verse.org/Simulation }
using { /UnrealEngine.com/Temporary/Diagnostics }

#───────────────────────────────────────────────────────────────────────────────
# Loadout struct
#───────────────────────────────────────────────────────────────────────────────

player_loadout := struct:
    OutfitID      : string
    BackBlingID   : string
    PickaxeID     : string
    GliderID      : string
    ContrailID    : string
    WrapID        : string
    EmoteSlots    : []string   # Up to 6 equipped emotes
    SpraySlots    : []string   # Up to 4 equipped sprays

DefaultLoadout() : player_loadout =
    player_loadout:
        OutfitID    := "Outfit_RamirezBase"
        BackBlingID := "BackBling_Default"
        PickaxeID   := "Pickaxe_Default"
        GliderID    := "Glider_Default"
        ContrailID  := ""
        WrapID      := ""
        EmoteSlots  := array{"", "", "", "", "", ""}
        SpraySlots  := array{"", "", "", ""}

#───────────────────────────────────────────────────────────────────────────────
# Loadout manager device
#───────────────────────────────────────────────────────────────────────────────

loadout_manager := class(creative_device):

    # Maps agent → their current loadout.
    # Verse maps are immutable; we re-create on each update.
    var PlayerLoadouts : [agent]player_loadout = map{}

    OnBegin<override>()<suspends> : void =
        Print("LoadoutManager: initialised.")

    # ── Public API ─────────────────────────────────────────────────────────────

    # Retrieves the loadout for an agent. Returns the default loadout if none set.
    GetLoadout(Player : agent) : player_loadout =
        if (Loadout := PlayerLoadouts[Player]):
            Loadout
        else:
            DefaultLoadout()

    # Sets the full loadout for an agent and triggers a visual refresh.
    SetLoadout(Player : agent, NewLoadout : player_loadout) : void =
        set PlayerLoadouts = PlayerLoadouts.Set(Player, NewLoadout)
        RefreshVisuals(Player, NewLoadout)

    # Updates a single slot without replacing the whole loadout.
    SetOutfit(Player : agent, OutfitID : string) : void =
        Old := GetLoadout(Player)
        New := player_loadout:
            OutfitID    := OutfitID
            BackBlingID := Old.BackBlingID
            PickaxeID   := Old.PickaxeID
            GliderID    := Old.GliderID
            ContrailID  := Old.ContrailID
            WrapID      := Old.WrapID
            EmoteSlots  := Old.EmoteSlots
            SpraySlots  := Old.SpraySlots
        SetLoadout(Player, New)

    SetPickaxe(Player : agent, PickaxeID : string) : void =
        Old := GetLoadout(Player)
        New := player_loadout:
            OutfitID    := Old.OutfitID
            BackBlingID := Old.BackBlingID
            PickaxeID   := PickaxeID
            GliderID    := Old.GliderID
            ContrailID  := Old.ContrailID
            WrapID      := Old.WrapID
            EmoteSlots  := Old.EmoteSlots
            SpraySlots  := Old.SpraySlots
        SetLoadout(Player, New)

    SetBackBling(Player : agent, BackBlingID : string) : void =
        Old := GetLoadout(Player)
        New := player_loadout:
            OutfitID    := Old.OutfitID
            BackBlingID := BackBlingID
            PickaxeID   := Old.PickaxeID
            GliderID    := Old.GliderID
            ContrailID  := Old.ContrailID
            WrapID      := Old.WrapID
            EmoteSlots  := Old.EmoteSlots
            SpraySlots  := Old.SpraySlots
        SetLoadout(Player, New)

    SetGlider(Player : agent, GliderID : string) : void =
        Old := GetLoadout(Player)
        New := player_loadout:
            OutfitID    := Old.OutfitID
            BackBlingID := Old.BackBlingID
            PickaxeID   := Old.PickaxeID
            GliderID    := GliderID
            ContrailID  := Old.ContrailID
            WrapID      := Old.WrapID
            EmoteSlots  := Old.EmoteSlots
            SpraySlots  := Old.SpraySlots
        SetLoadout(Player, New)

    # ── Visual application ─────────────────────────────────────────────────────

    # Called whenever a loadout changes. Dispatches to the C++ cosmetic manager
    # via the custom event interface on the character.
    RefreshVisuals(Player : agent, Loadout : player_loadout) : void =
        if (FortChar := Player.GetFortCharacter[]):
            # The actual mesh swap is done on the C++ side by UFortCosmeticManager.
            # We trigger it here by calling the character's exposed BP event.
            Print("LoadoutManager: Applying loadout for {Player} — Outfit={Loadout.OutfitID}")
            # In a full implementation this would call a BP function on FortChar.
            # Placeholder:
            # FortChar.ApplyLoadout(Loadout.OutfitID, Loadout.BackBlingID, Loadout.PickaxeID)
