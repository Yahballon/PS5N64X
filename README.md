<div align="center">

# N64XY

**64-bit classics. Next-generation PS5 homebrew.**

**v1.0.8 · by YAHBALLON · PPSA99206**

[![N64XY PS5 native build](https://github.com/Yahballon/PS5N64X/actions/workflows/build-n64xy.yml/badge.svg)](https://github.com/Yahballon/PS5N64X/actions/workflows/build-n64xy.yml)
[![Releases](https://img.shields.io/github/v/release/Yahballon/PS5N64X?style=flat-square)](https://github.com/Yahballon/PS5N64X/releases)

</div>

**N64XY** is a ParaLLEl-N64 based native Nintendo 64 homebrew emulator for a compatible jailbroken PS5, with a brand new Vulkan 3D CoverFlow frontend. The original **ParaLLEl N64**, cached CPU interpreter, Angrylion graphics and HLE RSP are retained. The repository remains named PS5N64X to preserve existing links.

## Features

| | |
| --- | --- |
| **3D CoverFlow** | Original Porpoise Vulkan rendering, neon glass cases, reflections and scrolling |
| **5 exclusive themes** | Quantum Lime, Nebula Purple, Retro Red, Ice Crystal, Golden Era |
| **Animated boot** | Original 64-bit Orbital Gateway: independent orbit rings, floating satellites and actual ROM/USB/NFS scan status |
| **Box art** | Local cache first; legacy region-matched N64 artwork, [Libretro direct thumbnail index](https://thumbnails.libretro.com/Nintendo%20-%20Nintendo%2064/Named_Boxarts/) and upstream mirror |
| **True case ratio** | Automatic square, landscape and portrait 3D boxes, entire cover visible, uncropped |
| **Game presentation** | 16:9 Fullscreen default, 4:3 Original or Integer pixels |
| **Native exit** | ○ in library → × Yes / ○ No; proper `sceSystemServiceLoadExec("exit", …)` |

## Install

Download `PPSA99206.zip` from [GitHub Releases](https://github.com/Yahballon/PS5N64X/releases) when the signed build has been published. Extract the full directory and install it using a compatible PS5 homebrew launcher.

**Updating does not require deleting your old data.** Title ID stays **PPSA99206**, and games, saves, settings, cached art and logs stay under `/data/PS5N64/`. Put your own legally acquired Nintendo 64 files under `/data/PS5N64/games/`. Existing compatible USB and NFS sources are preserved.

## Settings and controllers

Browse with D-pad / left stick; **×** plays, **□** opens settings, and **○** opens the exit confirmation. N64 Controller mapping, C buttons, analog stick, saves and shortcuts remain configurable. Choose the new Themes under **Settings → Appearance**, and adjust aspect mode under **Settings → Display**. For widescreen filling, use **16:9 Fullscreen** (the original 4:3 game image is stretched, not cropped).

## Build from source

The [N64XY Native Build workflow](.github/workflows/build-n64xy.yml) installs a PS5 SDK/Vulkan toolchain, compiles the original ParaLLEl N64 core and native Porpoise UI, signs `eboot.bin`, checks `PPSA99206.zip` and publishes an official release on successful `main` builds. It verifies the previously released title-specific loader helper by SHA256; no commercial ROMs or BIOS files are downloaded.

```text
n64/src/                        ParaLLEl-N64 runtime and Orbital Gateway loader
n64/third_party/                Retained original ParaLLEl N64 core
n64/tools/                      PS5 cross-build and package checks
ps5/frontend/                   CoverFlow, covers, controller & settings
ps5/porpoise_original/          Original Porpoise shader and Vulkan renderer (licensed)
ps5/frontend/assets/            Embedded N64XY emblem
tests/                          Regression tests
```

**Credits:** Created/maintained by **YAHBALLON**. Built on [ParaLLEl N64](https://github.com/libretro/parallel-n64), licensed Porpoise and the existing open-source PS5 homebrew SDK. Not affiliated with Nintendo or Sony. Bring your own legal game backups.
