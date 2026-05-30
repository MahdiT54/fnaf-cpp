
https://github.com/user-attachments/assets/5d782dc5-a844-415d-862d-262339d6e550

# Survive2Sunrise

<p align="center">
  <a href="https://licwebsolutions.com"><strong>LIC Web Solutions</strong></a><br>
  <sub>Web design &amp; development · Long Island City, NY</sub>
</p>

A C++17 terminal survival game built with ncurses.

> **Disclaimer:** This is an independent fan project inspired by the survival-horror genre and *Five Nights at Freddy's* mechanics. It is not affiliated with, endorsed by, or connected to *Five Nights at Freddy's* or its rights holders.

Survive until sunrise while managing limited battery power. Freddo approaches from the left, Chico from the right — close the correct doors to block them, but every closed door drains extra battery.

**Freddo & Chico** — They used to headline the pizzeria stage (Freddo on vocals, Chico on side acts). After closing they still roam: Freddo through the western halls toward your left door, Chico around the east and kitchens toward your right. Hold the matching door long enough and they retreat, but closed doors cost extra battery — and they get bolder as the night goes on.

## Play (no coding required)

Download a zip from [GitHub Releases](https://github.com/MahdiT54/fnaf-cpp/releases), extract the folder, and use the launcher inside:

| Platform | Download | How to start |
|----------|----------|--------------|
| **Windows** | `console-fnaf-windows.zip` | Double-click **`PLAY.bat`** |
| **macOS** | `console-fnaf-macos.zip` | Double-click **`Play.command`** |

See `HOW_TO_PLAY.txt` (Windows) or `HOW_TO_PLAY_mac.txt` (macOS) inside the zip for controls and troubleshooting.

**macOS first launch:** if macOS blocks the launcher, right-click **`Play.command`** → **Open** → **Open** once.

The macOS zip is built on a Mac with `scripts/package-macos.sh` before upload (same idea as the Windows packager).

## Controls

- `A`: Toggle the left door
- `D`: Toggle the right door
- `Q`: Quit

## Build from source

### Requirements

- C++17 compiler
- CMake 3.16+
- ncurses

### Windows (MSYS2)

CMake expects MSYS2 UCRT64 ncurses at `C:/msys64/ucrt64` by default. Override if needed:

```sh
cmake -S . -B build -DNCURSES_ROOT=/path/to/ucrt64
cmake --build build
```

Run from an MSYS2/UCRT64 terminal or another ncurses-compatible terminal.

### macOS / Linux

```sh
brew install cmake ncurses   # macOS
cmake -S . -B build
cmake --build build
./build/console_fnaf
```

## Project structure

- `src/main.cpp` - game loop and ncurses setup
- `src/game_state.*` - shared state and time/battery logic
- `src/ai.*` - animatronic AI (Freddo, Chico)
- `src/input.*` - keyboard handling
- `src/render.*` - terminal UI
- `scripts/package-windows.ps1` - Windows release packaging
- `scripts/package-macos.sh` - macOS release packaging (run on a Mac)
- `release/` - launchers (`PLAY.bat`, `Play.command`) and player instructions

## Contributing

Contributions are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md) for how to fork, build, and open a pull request.

## License

This project is licensed under the [MIT License](LICENSE).

---

<p align="center">
  <a href="https://licwebsolutions.com"><strong>L I C</strong></a><br>
  <strong><a href="https://licwebsolutions.com">Web Solutions</a></strong><br>
  <sub><a href="https://licwebsolutions.com">licwebsolutions.com</a> · <a href="https://github.com/LIC-Web-Solutions">GitHub</a> · hello@licwebsolutions.com</sub>
</p>
