# FNGameProj — Fortnite Chapter 1 Season 9 Recreation

A UEFN (Unreal Editor for Fortnite) project with three interconnected purposes:
1. Recreate the Chapter 1 Season 9 (v9.10) gameplay experience
2. Author and preview **custom cosmetics** (skins, back blings, pickaxes, gliders, wraps, emotes)
3. **Backport cosmetics** from any Fortnite season/chapter into the S9 visual and rig format

## Overview

This project faithfully recreates the core gameplay loop, weapons, mechanics, and UI of Fortnite Battle Royale as it existed during Chapter 1 Season 9 (patch v9.10). On top of that foundation it ships a full cosmetics authoring and backport pipeline so that custom or post-S9 cosmetics display correctly on the Chapter 1 Season 9 character rig.

## Systems Implemented

### Core Gameplay
- Battle Royale storm system with S9 ring timing
- 100-player lobby management
- Loot spawning and distribution (S9 loot pool)
- Elimination tracking and placement

### Movement
- Sprint, crouch, prone-roll, slide (S9 mechanics)
- Building placement (Wall, Floor, Ramp, Roof)
- Edit system
- Vehicle entry/exit

### Weapons (Season 9 Pool)
- **Assault Rifles**: Assault Rifle (Common–Legendary), Burst AR, Infantry Rifle, Heavy AR
- **SMGs**: SMG, Tactical SMG, Compact SMG
- **Shotguns**: Combat Shotgun, Tactical Shotgun, Pump Shotgun (vaulted), Heavy Shotgun
- **Snipers**: Bolt-Action Sniper, Semi-Auto Sniper, Suppressed Sniper
- **Pistols**: Pistol, Suppressed Pistol, Dual Pistols, Hand Cannon
- **LMGs**: Light Machine Gun
- **Explosives**: Rocket Launcher, Grenade Launcher, Quad Launcher, Proximity Grenade Launcher
- **Special**: Minigun, Stink Bow (new S9)

### Consumables
- Medkit, Bandages, Chug Jug, Shield Potion, Small Shield Potion
- Slurp Juice, Pepper, Hop Rock (vaulted in S9 but referenced)
- Mushrooms, Bush

### Building
- Wood / Stone / Metal materials
- Wall, Floor, Ramp, Roof piece types
- Edit mode with pre-set shapes
- Build cost per piece
- In-game Building Settings menu on F10, using the supplied plus logo
- Edit On Release, Confirm Edit On Release, Reset On Release, Simple Edit, and Builder Pro options save and apply during play

### Vehicles
- Shopping Cart, All-Terrain Kart (ATK), Quadcrasher, Baller
- Drift board (X-4 Stormwing in S9)

### UI
- HUD matching S9 visual style
- Inventory grid (5 slots)
- Minimap
- Elimination feed
- Build mode radial

## Project Structure

### Current `Content` folders

```text
Content/
├── Abilities/
├── AI/
├── Animation/
├── Athena/
├── Audio/
├── Balance/
├── Banners/
├── Building/
├── Characters/
├── Collections/
├── Cosmetics/
├── DataTables/
├── Developer/
├── Developers/
│   └── samiy/
│       └── Collections/
├── Effects/
├── Environments/
├── Events/
├── Gadgets/
├── GameModes/
├── Gameplay/
├── Inventory/
├── Items/
├── Limited/
├── Maps/
│   ├── Athena/
│   └── Frontend/
├── Materials/
├── Meshes/
├── Niagara/
├── Particles/
├── Physics/
├── Playlists/
├── Quests/
├── Sounds/
├── Splash/
│   └── EdSplash.png
├── UI/
│   └── Brand/
│       └── ProjectLogo.png
├── Valkyrie/
├── Vehicles/
├── Verse/
│   ├── CosmeticSystem/
│   └── GameSystems/
├── Weapons/
└── World/
```

Most category folders are placeholders and contain no Fortnite assets or Unreal map files.
`Content/Splash/EdSplash.png` is the project-specific Unreal Editor splash image.
To print the current tree in the VS Code terminal from the project root, run:

```powershell
tree Content /A
```

The structure below is a target layout, not a list of assets currently present.

```
FortniteGame/
├── Config/                        # Engine and game configuration
├── Content/
│   ├── Blueprints/
│   │   ├── Characters/            # Player character BP
│   │   ├── Weapons/               # Weapon base + per-weapon BPs
│   │   ├── Consumables/           # Consumable BPs
│   │   ├── Vehicles/              # Vehicle BPs
│   │   ├── Building/              # Building piece BPs
│   │   ├── GameMode/              # Game mode + game state BPs
│   │   ├── UI/                    # Widget blueprints
│   │   └── Abilities/             # Gameplay Ability BPs
│   ├── Cosmetics/
│   │   ├── Characters/            # Custom character / outfit assets
│   │   │   ├── _Template/         # Base template for new outfits
│   │   │   └── Examples/          # Example backported skins
│   │   ├── BackBlings/            # Back bling assets
│   │   ├── Pickaxes/              # Harvesting tool assets
│   │   ├── Gliders/               # Glider/umbrella assets
│   │   ├── Emotes/                # Emote animation assets
│   │   ├── Sprays/                # Spray assets
│   │   ├── LoadingScreens/        # Loading card assets
│   │   ├── Wraps/                 # Weapon/vehicle wrap materials
│   │   ├── Contrails/             # Skydive contrail assets
│   │   ├── Pets/                  # Pet back bling assets
│   │   ├── Toys/                  # Toy item assets
│   │   └── Skydiving/             # Skydiving trail assets
│   ├── DataTables/                # Weapon stats, loot pools, item data,
│   │   │                          #   cosmetic registries
│   │   ├── DT_CosmeticRegistry.json
│   │   ├── DT_BackportMapping.json
│   │   └── DT_WeaponData.json
│   ├── Maps/                      # Level files
│   ├── Materials/
│   │   ├── Cosmetics/             # Master materials for cosmetic slots
│   │   │   ├── M_CharacterBase    # Base character material (S9 style)
│   │   │   ├── M_HairBase         # Hair master material
│   │   │   ├── M_WrapBase         # Wrap master material
│   │   │   └── MI_*/              # Material instances per cosmetic
│   │   └── Shared/                # Shared utility materials
│   ├── Rigs/
│   │   ├── S9_BaseSkeleton        # Chapter 1 S9 base skeleton
│   │   └── RetargetProfiles/      # IK retarget profiles for backporting
│   └── Verse/                     # Verse scripts for UEFN logic
│       ├── GameSystems/
│       └── CosmeticSystem/
├── Plugins/
│   ├── FNGamePlugin/              # Gameplay systems plugin
│   └── FNCosmeticsPlugin/         # Cosmetics & backport pipeline plugin
└── Source/
    └── FortniteGame/              # C++ source modules
```

## Compatibility

- Unreal Engine 4.23 (the local C++ build target validated for this project)
- Unreal Engine 5 and UEFN have not been validated

## Setup

1. Open `FortniteGame.uproject` with Unreal Engine 4.23.
2. Build the `FortniteEditor` target and enter Play-in-Editor from a level that spawns `AAthenaCharacter`.
3. Press **F10** to open the Building Settings menu; press **Esc** to close it.

No `.umap` level asset is currently included, so the menu is available once a playable level is supplied or created.

## Editor layout

The editor panel layout is stored in Unreal Editor preferences rather than in the project. To match the reference layout, use **Window → Load Layout → Default Editor Layout**, then dock **Place Actors / World Outliner** on the left, **Details** on the right, and **Content Browser** along the bottom. Enable the Content Browser **Sources** panel to show the project folders.

## Visual Studio Solution

Open `FortniteGame.sln` in Visual Studio and build the `FortniteEditor` target for `Development Editor | Win64`. After a successful editor build, its project module DLLs (matching `UE4Editor-*.dll`) are copied from `Binaries/Win64` into the project-root `x64` folder. These are Unreal Editor module DLLs, not standalone game runtime DLLs. The `FortniteGame` target builds the standalone `FortniteGame.exe`.

The project has been compiled successfully with the local Unreal Engine 4.23 installation. The editor DLL build command is:

```powershell
& "C:\path\to\UnrealBuildTool.exe" FortniteEditor Win64 Development "-Project=C:\path\to\FortniteGame.uproject"
```

## Season 9 Authenticity Notes

- Pump Shotgun is **vaulted** (not in floor loot)
- Combat Shotgun replaces it as primary shotgun
- Slipstream wind tunnels referenced in map layout
- Neo Tilted / Mega Mall POIs included as brush geometry
- Storm timing matches S9 BR match pacing

---

## Cosmetics System

### Custom Cosmetic Authoring

The project ships a full cosmetics authoring pipeline targeting the Chapter 1 Season 9 art style and character rig.

**Supported cosmetic slots:**

| Slot | Folder | Description |
|---|---|---|
| Outfit / Character | `Content/Cosmetics/Characters/` | Full body skin |
| Back Bling | `Content/Cosmetics/BackBlings/` | Back attachment |
| Harvesting Tool | `Content/Cosmetics/Pickaxes/` | Pickaxe mesh + anim |
| Glider | `Content/Cosmetics/Gliders/` | Deploy + glide meshes |
| Emote | `Content/Cosmetics/Emotes/` | Full-body animation |
| Spray | `Content/Cosmetics/Sprays/` | Decal texture |
| Loading Screen | `Content/Cosmetics/LoadingScreens/` | 2D card artwork |
| Wrap | `Content/Cosmetics/Wraps/` | Material override |
| Skydive Contrail | `Content/Cosmetics/Contrails/` | Niagara trail |
| Pet | `Content/Cosmetics/Pets/` | Animated back bling |
| Toy | `Content/Cosmetics/Toys/` | Throwable prop |

**Authoring a new outfit:**
1. Duplicate `Content/Cosmetics/Characters/_Template/` and rename it.
2. Replace `SK_CustomCharacter` with your skeletal mesh **retargeted to `Rigs/S9_BaseSkeleton`**.
3. Create a material instance from `Materials/Cosmetics/M_CharacterBase` for each material slot.
4. Populate `DT_CosmeticRegistry` with the new row.
5. Set the `PreviewCosmeticActor` in the Cosmetic Preview map to load your new outfit.

### Backporting Cosmetics

Backporting converts a cosmetic authored for a later Fortnite rig (e.g. Chapter 2–5 proportions) into the Chapter 1 Season 9 skeleton so it displays correctly in this project.

**Pipeline:**

```
Source cosmetic mesh (any season)
        │
        ▼
  IK Retarget Profile
  (Content/Rigs/RetargetProfiles/)
        │
        ▼
  S9_BaseSkeleton-bound SK
        │
        ▼
  Material re-slot to M_CharacterBase instances
        │
        ▼
  Row added to DT_BackportMapping
        │
        ▼
  Registered in DT_CosmeticRegistry
```

**Retarget profiles provided:**

| Profile | Source rig |
|---|---|
| `IKR_Ch2_to_S9` | Chapter 2 / 3 rig → S9 |
| `IKR_Ch4_to_S9` | Chapter 4 / 5 rig → S9 |
| `IKR_OGSeason_to_S9` | Early Chapter 1 (pre-S5) → S9 |

**Step-by-step backport:**
1. Import the source season mesh as a Skeletal Mesh into `Content/Cosmetics/Characters/Backports/`.
2. Open the matching retarget profile in `Content/Rigs/RetargetProfiles/`.
3. Run **Export Retarget Asset** → save output to `Content/Cosmetics/Characters/Backports/<SkinName>/`.
4. Create material instances from `M_CharacterBase`; map diffuse/normal/specular from the source textures.
5. Add a row to `DT_BackportMapping` (source ID → backport asset path).
6. Add a row to `DT_CosmeticRegistry` to make the cosmetic selectable in the locker.

### Cosmetic Preview Map

Open `Content/Maps/CosmeticPreview` to preview any cosmetic in isolation without loading the full BR map. The map contains:
- A rotating mannequin actor driven by `BP_CosmeticPreviewActor`
- A light rig matching the Chapter 1 lobby aesthetic
- A widget (`WBP_CosmeticPicker`) to cycle through registered cosmetics at runtime

