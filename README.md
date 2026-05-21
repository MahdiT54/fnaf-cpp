# Survive2Sunrise

A C++17 terminal survival game built with ncurses.

Survive until sunrise while managing limited battery power. Freddo approaches from the left, Chico from the right - close the correct doors to block them, but every closed door drains extra battery.

## Play (Windows)

Download **`console-fnaf-windows.zip`** from [GitHub Releases](https://github.com/MahdiT54/fnaf-cpp/releases), extract the folder, and run **`PLAY.bat`**.

See `HOW_TO_PLAY.txt` in the release for controls.

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
- `release/` - launcher and player instructions for the Windows zip
