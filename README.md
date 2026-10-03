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
```

Restart the game after installing.

## Platform support

- Windows x64

Other Foxhollow platforms are not supported by this mod yet.

## Repository

https://github.com/saulob/Foxhollow-Fast-Memory-Card-Loading

## License

[MIT](LICENSE)
