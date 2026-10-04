# Foxhollow Fast Memory Card Loading

A standalone native mod for Star Fox Adventures running through Foxhollow that removes the original artificial Memory Card loading screen delay.

## What it does

When the game saves or loads, it first holds the Memory Card screen on purpose for a fixed delay, a leftover from the original GameCube release, and only then starts accessing the card. Foxhollow does not need that wait.

With this mod:

- The Memory Card screen still appears, with the same text.
- Card access starts immediately instead of after the fixed delay.
- Saving, loading, error messages and retries work exactly as in the normal game.

The mod removes only the wait before card access. The save or load itself still takes as long as it normally does.

This is the same behavior as the Foxhollow `feature/fast-memory-card-loading` branch, packaged as a mod so Foxhollow itself is not modified.

## Controls

None. The mod is always active while it is loaded.

## Installation

**Recommended:** install through the Foxhollow Launcher once the mod is published there.

**Manual:** place the extracted mod folder in the Foxhollow Launcher's `mods` folder, so it looks like this:

```
mods/
  com.saulob.fast-memory-card-loading/
    mod.json
    lib/
      windows-amd64/
        mod.dll
      linux-amd64/
        mod.so
      linux-arm64/
        mod.so
      macos-x86_64/
        mod.so
      macos-arm64/
        mod.so
```

Foxhollow only loads the library in the folder that matches your system and ignores the others, so you only need the folder for your platform.

Restart the game after installing.

## Platform support

| Platform | Folder | Status |
| --- | --- | --- |
| Windows x64 | `windows-amd64` | Tested in game |
| Linux x86_64 | `linux-amd64` | Build validated, in-game testing pending |
| Linux ARM64 | `linux-arm64` | Build validated by GitHub Actions, in-game testing pending |
| macOS Apple Silicon | `macos-arm64` | Build validated by GitHub Actions, in-game testing pending |
| macOS Intel | `macos-x86_64` | Build validated by GitHub Actions, in-game testing pending |

Official Foxhollow builds are currently published for Windows x64, Linux x86_64 and macOS Apple Silicon. The Linux ARM64 and macOS Intel libraries are for Foxhollow builds you compile yourself. Windows on ARM is not supported.

The mod needs no extra libraries on any platform. If the Foxhollow log shows `[Fast Memory Card Loading] disabled: ...`, the mod could not find or patch the game code it needs and left the game unchanged.

## Repository

https://github.com/saulob/Foxhollow-Fast-Memory-Card-Loading

## License

[MIT](LICENSE)
