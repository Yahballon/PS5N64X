# PS5N64 is a passion project, not a piracy project.
<p align="center">
  <img src="screenshots/background.png" alt="PS5NES game shelf">
</p>
PS5N64 is a native PS5 homebrew Nintendo 64 emulator using the **ParaLLEl N64 / Mupen64Plus core**.

It keeps the original PS5SX2-style shelf, animated game cases, cover glow, reflections, menu sounds and settings panel, with a Nintendo 64 backend.

The current renderer is **Angrylion** at native internal resolution.

## Features

A green, charcoal and gray Nintendo 64-inspired theme with the supplied launcher logo.

- Play **`.z64`**, **`.v64`** and **`.n64`** Nintendo 64 ROMs.
- Play ZIP archives containing **one supported N64 ROM**, decompressed directly into RAM.
- Load games from internal storage, USB or an **NFSv3 share**.
- Automatic Nintendo 64 box-art downloads and custom local covers.
- Original animated PS5SX2-style cover-flow game shelf.
- Cached NFS game library for fast repeat launches.
- Fresh local and USB scanning on startup.
- Battery-backed saves and **Controller Pak saves**.
- **One save-state slot per ROM**.
- Native analog-stick input.
- Right-stick C-button controls.
- Configurable digital buttons and emulator shortcuts.
- Global and per-game controller configurations.
- Stereo audio at **48 kHz**.
- Audio prebuffering, short underrun fades and bounded queue latency.
- Fast bounded IPv4 offline-server detection for configured NFS servers.
- Controller-operated NFS configuration.
- Local web settings.
- Bounded cover streaming for large libraries.
- No Nintendo boot firmware required.
- No executable-memory JIT is used.

## Install

1. Download **`PS5N64-v7-Audio-Fix-modded-by-YAHBALLON.zip`** from the release assets and extract it.

2. Copy/install the complete **`PPSA99206`** application folder using your compatible PS5 homebrew environment.

The application folder should be:

```text
/data/homebrew/PPSA99206/
```

3. Launch PS5N64 with your compatible PS5 homebrew launcher.

Keep every packaged application file.

Your existing games, saves, settings, covers and cache live separately under:

```text
/data/PS5N64/
```

For an update, replace the application files and restart PS5N64.

Refresh your homebrew launcher if it still displays the old icon or application information.

A normal retail PS5 cannot run this homebrew application without a compatible homebrew environment.

## Add games

### Internal storage

```text
/data/PS5N64/games/
```

### USB

Primary folder:

```text
/mnt/usb0/homebrew/PS5N64/games/
```

USB0 through USB7 are supported.

These alternative folders are also scanned:

```text
/mnt/usb0/homebrew/PS5N64/roms/
/mnt/usb0/PS5N64/games/
/mnt/usb0/N64/
```

Local and USB locations are freshly scanned when PS5N64 starts, even when an existing library cache is present.

### NFS

Open the **All games** settings and configure your NFS URL.

Example:

```text
nfs://192.168.0.87/N64
```

This address is only a configuration example and is not a tested public export.

PS5N64 connects directly to the configured **NFSv3** server.

NFS game listings use a local cache so that the application does not need to perform a full network scan on every startup.

A bounded IPv4 availability check is used so an offline NAS does not stall startup for an excessive amount of time.

If the configured NFS server is unavailable, cached NFS games and available local games can remain visible.

Configuration, cache, saves and logs are stored under:

```text
/data/PS5N64/
```

If `/data` is unavailable, PS5N64 can use:

```text
/download0/PS5N64/
```

as a writable fallback.

### ZIP support

A ZIP archive must contain exactly **one supported Nintendo 64 ROM**.

Supported ROM extensions inside ZIP archives:

```text
.z64
.v64
.n64
```

Stored and Deflate ZIP compression are supported.

Multi-ROM archives should be unpacked into individual ROM files or individual game ZIP archives.

## Default controls

### Game shelf

| Action | Default |
| --- | --- |
| Browse games | D-pad / left stick, left and right |
| Start game | Cross |
| Open settings | Square |
| This game / All games | L1 / R1 in settings |
| Settings / Controls tab | L2 / R2 in settings |

### In game

| N64 action | Default DualSense control |
| --- | --- |
| Analog stick | Left stick |
| C directions | Right stick |
| A | Cross |
| B | Square |
| C Up | Triangle |
| C Down | Circle |
| Z | L2 |
| L | L1 |
| R | R1 |
| Start | Options |
| D-pad | D-pad |

### Emulator shortcuts

| Emulator shortcut | Default combination |
| --- | --- |
| Save state | L3 + R3 + D-pad Up |
| Load state | L3 + R3 + D-pad Down |
| Return to shelf | L1 + R1 + Touchpad click |

## Customize your buttons

Digital buttons and emulator shortcuts can be configured globally or per game.

- Choose **All games** for global defaults.
- Choose **This game** for a per-game override.
- Analog movement uses the DualSense left stick.
- N64 C directions use the right stick by default.
- Save and load-state shortcuts can be remapped.
- The menu/exit shortcut can be remapped.
- A **two-second emergency hold** remains available for returning to the game shelf.
- Settings apply when the game is launched again.

One active controller is currently exposed by the PS5 frontend.

Saving a new state overwrites that ROM's previous state slot.

Battery-backed cartridge saves and Controller Pak data are stored separately from the application files.

## Library cache

PS5N64 maintains a cached game library for configured NFS shares.

Local and USB folders are scanned freshly at startup, even if an existing library cache is available.

This allows newly added or removed local games to appear without requiring a full NFS rescan.

Cached NFS entries help reduce startup delays and allow previously indexed network titles to remain visible when an IPv4 NAS is temporarily offline.

### Reset library

Open:

**All games → Settings → Reset library**

Use this after major changes to your library or when you want PS5N64 to rebuild its cached game information.

Restart PS5N64 after resetting the library.

ROMs and save files are **not deleted**.

### Clear NFS configuration

Open:

**All games → Settings → Clear all NFS URLs**

This removes the configured NFS addresses.

Restart PS5N64 after using this action.

Your ROMs and save files are retained.

## Covers

Automatic box-art downloads use the Nintendo 64 filename index from:

https://github.com/Globaleliteee/Gaming_Console_Covers_Art_Repository/tree/main/covers/nintendo/N64

The current repository contains an index of **801 Nintendo 64 front covers**.

Cover matching uses the ROM filename and understands title and region information.

Additional ROM revision or language tags do not prevent a valid match.

Existing failed cover lookups are retried after this update.

A **libretro-thumbnails fallback** is also available when an appropriate primary cover is not found.

For best results, use clean ROM filenames with recognizable game titles and region tags.

Downloaded artwork remains cached locally.

Only a bounded amount of nearby artwork is kept active in memory while browsing large libraries.

## Audio

PS5N64 outputs stereo audio at:

```text
48000 Hz
```

v7 introduces additional audio handling intended to reduce crackling caused by short emulation stalls.

This includes:

- Audio prebuffering.
- Short underrun fades.
- Recovery after short stalls.
- Bounded audio queue latency.

Audio cannot compensate for sustained slow emulation.

If emulation remains below full speed for an extended period, audio interruptions or crackling can still occur.

## Renderer and performance

The current PS5N64 build uses:

- **ParaLLEl N64 / Mupen64Plus**
- Cached interpreter
- HLE RSP
- **Angrylion software rendering**
- Native **1x internal rendering resolution**
- 4K PS5 display output

The PS5 can output the application at 4K, but the Nintendo 64 scene itself is currently rendered internally at native resolution and then presented by the frontend.

**ParaLLEl-RDP upscaling is not currently integrated.**

**Gopher64 is not currently integrated.**

No executable-memory JIT is enabled in this build.

Performance varies between games.

Full-speed playback is **not guaranteed** and commercial-game compatibility still requires additional testing on real PS5 hardware.

## Current limits

- One active controller is currently exposed.
- Multiplayer is not exposed by the current frontend.
- 64DD is not exposed.
- Transfer Pak is not exposed.
- VRU is not exposed.
- Rumble is not exposed.
- ParaLLEl-RDP upscaling is not integrated.
- Gopher64 is not integrated.
- The current renderer is Angrylion at 1x internal resolution.
- No executable-memory JIT is enabled.
- General firmware compatibility has not been established.
- Performance varies by game.
- Full-speed playback is not guaranteed.
- Sustained slow emulation can interrupt audio.
- Broader real-console testing is still required.
- No copyrighted ROMs, Nintendo firmware or console keys are included.

## Latest changes

### v7 — by YAHBALLON

- Interface is now **English regardless of the PS5 console language**.
- Local and USB folders receive a fresh scan even when a library cache already exists.
- Added fast bounded IPv4 offline-NAS detection.
- Cached NFS games remain available when the configured network server is unavailable.
- Local games remain available when an NFS server is offline.
- Added **Reset library** to the All games settings.
- Added **Clear all NFS URLs** to the All games settings.
- ROMs and saves are retained when using these reset actions.
- Added audio prebuffering.
- Added audio underrun recovery.
- Added short underrun fades to reduce audible crackling from temporary stalls.
- Added bounded audio queue latency.
- Menu version label updated to:

```text
v7 by YAHBALLON
```

## App data and logs

PS5N64 stores application data separately from the installed application.

Primary data folder:

```text
/data/PS5N64/
```

Writable fallback:

```text
/download0/PS5N64/
```

Typical content includes:

| Content | Location |
| --- | --- |
| Games | `/data/PS5N64/games/` |
| Saves and states | `/data/PS5N64/saves/` |
| Settings | `/data/PS5N64/` |
| Covers | `/data/PS5N64/covers/` |
| Library cache | `/data/PS5N64/cache/` |
| Logs | `/data/PS5N64/logs/` |

Exact files may vary between builds.

## Build from source

Prepare the required dependencies and run:

```bash
PS5N64_DEPS=/path/to/prepared/deps bash n64/tools/build.sh
```

The build uses the existing public PS5 development environment together with components including:

- PS5 SDK/toolchain
- PS5_Vulkan
- libnfs
- Static HTTPS dependencies
- ParaLLEl N64 / Mupen64Plus components
- Angrylion renderer

Core objects and frontend objects are built outside their source directories.

The current emulator configuration uses a cached interpreter, HLE RSP and Angrylion software rendering.

No executable-memory JIT is enabled.

## Host tests

On Linux with **Clang 18**, run:

```bash
python3 n64/tools/test.py
```

The supplied host test uses original test code and does **not** contain commercial Nintendo games or Nintendo boot firmware.

The test generates original MIPS cartridge code and validates:

- ROM startup.
- Framebuffer output.
- Audio DMA.
- Analog input.
- C-button input.
- Battery-backed saves.
- Save-state roundtrip.
- Loading a second ROM.

Passing the host tests does not guarantee compatibility or full performance on real PS5 hardware.

Real-console testing remains necessary.

## Development notes

PS5N64 is based on the PS5SNES/PS5SX2-style frontend structure but uses a separate Nintendo 64 emulator backend.

The original project scaffold and frontend components are retained where useful while Nintendo 64 emulation is handled through the N64 core.

Development and source modifications were **AI-assisted**.

AI-assisted development does not replace hardware validation, compatibility testing or review of the upstream emulator implementations.

## Credits and license

- **YAHBALLON** — project customization, integration and supplied assets.
- **Spyros and PCSX2 contributors** — original PS5SX2 interface and project scaffold.
- **ParaLLEl N64 / Mupen64Plus contributors** — Nintendo 64 emulation.
- **Angrylion contributors** — Nintendo 64 software rendering.
- **Libretro contributors** — emulator ecosystem and supporting components.
- [mihawk-99 / PS5_Vulkan](https://github.com/mihawk-99/PS5_Vulkan) — native Vulkan driver and runtime work.
- [Globaleliteee / Gaming Console Covers Art Repository](https://github.com/Globaleliteee/Gaming_Console_Covers_Art_Repository) — Nintendo 64 cover-art index.
- libnfs and the bundled networking/library authors.
- All additional third-party component authors and contributors.

When redistributing PS5N64, preserve the upstream notices and included GPL/component licenses.

Upstream components retain their original licenses.

Source modifications were AI-assisted.

## Disclaimer

PS5N64 is an independent homebrew project.

It is **not affiliated with, sponsored by or endorsed by Nintendo, Sony Interactive Entertainment, PCSX2, Mupen64Plus, ParaLLEl N64 or their respective contributors**.

"PlayStation", "PS5", "Nintendo", "Nintendo 64" and related names and trademarks belong to their respective owners.

Game names, artwork and other copyrighted material belong to their respective owners.

No copyrighted commercial ROMs, Nintendo firmware or console keys are distributed with PS5N64.

Please use your own legally obtained games.
