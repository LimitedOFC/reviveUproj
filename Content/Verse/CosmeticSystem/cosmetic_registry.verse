################################################################################
# cosmetic_registry.verse
#
# Runtime cosmetic registry for the UEFN / Verse layer.
# Mirrors DT_CosmeticRegistry for use inside Verse device logic.
# Verse cannot directly read UDataTable rows, so the relevant data is
# duplicated here as typed structs and a module-level map.
#
# Chapter 1 Season 9 — FNGameProj
################################################################################

using { /Fortnite.com/Devices }
using { /Verse.org/Simulation }
using { /UnrealEngine.com/Temporary/Diagnostics }

#───────────────────────────────────────────────────────────────────────────────
# Enums
#───────────────────────────────────────────────────────────────────────────────

cosmetic_slot := enum:
    Character
    BackBling
    Pickaxe
    Glider
    Emote
    Spray
    LoadingScreen
    Wrap
    Contrail
    Pet
    Toy

backport_state := enum:
    Native       # Authored natively for S9 rig
    Retargeted   # Retargeted from a later-season rig
    Pending      # Imported, retarget not yet complete

cosmetic_rarity := enum:
    Common
    Uncommon
    Rare
    Epic
    Legendary
    Icon
    Dark
    Frozen
    Lava
    Shadow
    Slurp
    Custom

source_season := enum:
    Chapter1_S1
    Chapter1_S2
    Chapter1_S3
    Chapter1_S4
    Chapter1_S5
    Chapter1_S6
    Chapter1_S7
    Chapter1_S8
    Chapter1_S9      # Native season for this project
    Chapter1_S10
    Chapter2
    Chapter3
    Chapter4
    Chapter5
    CustomSeason

#───────────────────────────────────────────────────────────────────────────────
# Cosmetic entry struct
#───────────────────────────────────────────────────────────────────────────────

cosmetic_entry := struct:
    CosmeticID    : string
    DisplayName   : string
    Description   : string
    Slot          : cosmetic_slot
    Rarity        : cosmetic_rarity
    SourceSeason  : source_season
    BackportState : backport_state
    # Asset paths are resolved by UFortCosmeticManager on the C++ side.
    # In Verse we only need the ID to dispatch to that system.
    bIsCustom     : logic
    bIsBackported : logic

#───────────────────────────────────────────────────────────────────────────────
# Registry — add one entry per cosmetic
#───────────────────────────────────────────────────────────────────────────────

# Module-level registry map: CosmeticID -> cosmetic_entry
cosmetic_registry_data : []cosmetic_entry = array:
    cosmetic_entry:
        CosmeticID    := "Outfit_RamirezBase"
        DisplayName   := "Ramirez"
        Description   := "Default female outfit. Chapter 1 Season 9 native."
        Slot          := cosmetic_slot.Character
        Rarity        := cosmetic_rarity.Common
        SourceSeason  := source_season.Chapter1_S9
        BackportState := backport_state.Native
        bIsCustom     := false
        bIsBackported := false
    cosmetic_entry:
        CosmeticID    := "Outfit_JonesyBase"
        DisplayName   := "Jonesy"
        Description   := "Default male outfit. Chapter 1 Season 9 native."
        Slot          := cosmetic_slot.Character
        Rarity        := cosmetic_rarity.Common
        SourceSeason  := source_season.Chapter1_S9
        BackportState := backport_state.Native
        bIsCustom     := false
        bIsBackported := false
    cosmetic_entry:
        CosmeticID    := "Pickaxe_Default"
        DisplayName   := "Default Pickaxe"
        Description   := "Standard harvesting tool."
        Slot          := cosmetic_slot.Pickaxe
        Rarity        := cosmetic_rarity.Common
        SourceSeason  := source_season.Chapter1_S9
        BackportState := backport_state.Native
        bIsCustom     := false
        bIsBackported := false
    cosmetic_entry:
        CosmeticID    := "Glider_Default"
        DisplayName   := "Default Glider"
        Description   := "Standard glider."
        Slot          := cosmetic_slot.Glider
        Rarity        := cosmetic_rarity.Common
        SourceSeason  := source_season.Chapter1_S9
        BackportState := backport_state.Native
        bIsCustom     := false
        bIsBackported := false
    # ── Add backported cosmetics below this line ──────────────────────────────
    # Duplicate the block above for each backported skin.
    # Set bIsBackported := true and BackportState := backport_state.Retargeted
    # once the IK retarget step is complete.

#───────────────────────────────────────────────────────────────────────────────
# Registry lookup helpers
#───────────────────────────────────────────────────────────────────────────────

# Returns the cosmetic_entry for a given ID, or false if not found.
FindCosmetic(ID : string)<decides> : cosmetic_entry =
    for (Entry : cosmetic_registry_data):
        if (Entry.CosmeticID = ID):
            return Entry
    false

# Returns all entries for a given slot.
GetCosmeticsForSlot(Slot : cosmetic_slot) : []cosmetic_entry =
    for (Entry : cosmetic_registry_data, Entry.Slot = Slot):
        Entry

# Returns all backported entries.
GetBackportedCosmetics() : []cosmetic_entry =
    for (Entry : cosmetic_registry_data, Entry.bIsBackported = true):
        Entry

# Returns all custom (fan-made) entries.
GetCustomCosmetics() : []cosmetic_entry =
    for (Entry : cosmetic_registry_data, Entry.bIsCustom = true):
        Entry
