# Lingering Subtitles Fix
A lightweight SKSE plugin that fixes subtitles that refuse to disappear after conversations end. Configure the cleanup behavior via INI file.

## What it does
When an NPC finishes talking, their subtitle should vanish. Sometimes it doesn't — lingering on screen through combat, fast travel, even into the next conversation. This plugin hooks into the engine and cleans them up.

## Features
- **Per-frame cleanup pass** (configurable interval)  
  Scans the active subtitle list at a set interval and removes any entries whose speaker has finished talking, died, or is otherwise gone. Runs as native engine-level code, not Papyrus.

- **Death state handling**  
  Catches actors mid-death-animation (e.g. bone dragons) where `IsDead()` returns false but the actor is already effectively gone. Uses life state directly for accurate detection.

- **Ghost actor support**  
  Ghosts and Wispmothers fade out rather than transitioning life states normally. The plugin handles these separately and removes their subtitles as soon as they stop speaking or leave the alive state.

- **Death alternative mod compatibility**  
  Mods that revive the player mid-death can corrupt nearby NPC actor state, leaving subtitles with no valid owner. A timestamp fallback catches these as a last resort and force removes them after a configurable timeout.

- **Dialogue menu awareness**  
  Skips the speaking-done check while a dialogue menu is open, preventing conflicts with proxy-based dialogue systems like Simply Order Summons.

## Requirements
- [Skyrim Script Extender](https://skse.silverlock.org/)
- [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)

## Configuration
Edit manually:

```
Data/SKSE/Plugins/LingeringSubtitlesFix.ini
```

| Setting | Default | Description |
|---|---|---|
| `fCleanupInterval` | `0.5` | How often (in seconds) the cleanup pass runs |
| `fStuckSubtitleTimeout` | `25.0` | How long a subtitle can linger before being force removed |

## Compatibility
- No ESP file, no load order impact
- Safe to install and uninstall mid-playthrough
- Compatible with subtitle altering mods like Subtitles / Floating Subtitles
- Compatible with death alternative mods via timestamp fallback

## Build

To clone the repository inside your current folder:

Open a terminal instance inside the folder and run:
```
git clone https://github.com/Arisuzea/LingeringSubtitlesFix .
```

This plugin is built with [CLibDT](https://www.nexusmods.com/skyrimspecialedition/mods/154240).

But if you do not want to use it:

SE + AE
```
xmake f -y -m release --toolchain=msvc --skyrim_se=y --skyrim_ae=y
xmake build
```

SE Only
```
xmake f -y -m release --toolchain=msvc --skyrim_se=y
xmake build
```

AE Only
```
xmake f -y -m release --toolchain=msvc --skyrim_ae=y
xmake build
```

VR Only
```
xmake f -y -m release --toolchain=msvc --skyrim_vr=y
xmake build
```
> **Note:** VR vtable offsets are unverified. The VR build exists but is untested.

## Credits
Made with [CLibDT](https://www.nexusmods.com/skyrimspecialedition/mods/154240).

- [CommonLibVR](https://github.com/alandtse/CommonLibVR)  
  The SKSE library this plugin is built on
