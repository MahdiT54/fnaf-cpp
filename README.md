# Console FNAF

A C++17 terminal survival game inspired by Five Nights at Freddy's, built with ncurses.

I started this as a frontend developer learning C++ by building a complete game loop from scratch: real-time input, terminal rendering, battery management, enemy AI, and win/loss conditions.

## Try it (no install)

**Easiest for players:** download the latest **`console-fnaf-windows.zip`** from [GitHub Releases](https://github.com/MahdiT54/fnaf-cpp/releases), extract the folder, and double-click **`PLAY.bat`**.

No compiler, CMake, or MSYS2 required. See `HOW_TO_PLAY.txt` inside the zip for controls.

**Easiest for viewers (CV, LinkedIn):** watch the demo clip below — most people will not build from source.

## Demo

Demo media is coming soon. Add a 15–30 second gameplay GIF or video here before sharing publicly.

Recommended launch assets:

- A gameplay GIF or short video (place it here)
- A screenshot of the terminal map
- A link to the Windows release zip

## Gameplay

Survive until 6 AM while managing limited battery power.

Freddo approaches from the left side. Chico approaches from the right side. Closing the correct door can block them, but every closed door drains extra battery. If the battery dies or an animatronic reaches you while the door is open, the shift ends.

## Controls

- `A`: Toggle the left door
- `D`: Toggle the right door
- `Q`: Quit

## For developers — build from source

### Build Requirements

- C++17 compiler
- CMake 3.16 or newer
- ncurses

On this Windows setup, the CMake file expects MSYS2 UCRT64 ncurses at:

```text
C:/msys64/ucrt64
```

You can override that path while configuring:

```sh
cmake -S . -B build -DNCURSES_ROOT=/path/to/ucrt64
```

### Build And Run

Configure and build:

```sh
cmake -S . -B build
cmake --build build
```

Run:

```sh
./build/console_fnaf.exe
```

On Windows, run the executable from an MSYS2/UCRT64 terminal or another terminal with ncurses terminfo available. Plain non-interactive PowerShell sessions may exit with `ncurses: cannot initialize terminal type`.

## What I Practiced

- Building a real-time game loop
- Using `std::chrono` for frame timing
- Modeling game state with C++ structs
- Passing state by reference and const reference
- Using inheritance and virtual functions for enemy AI
- Rendering a UI with ncurses
- Building native C++ with CMake

## Source Layout

- `src/main.cpp`: ncurses setup and the main game loop.
- `src/game_state.hpp` / `game_state.cpp`: Shared game data and hour/battery tick logic.
- `src/ai.hpp` / `ai.cpp`: Animatronic AI base class, Freddo, and Chico.
- `src/input.hpp` / `input.cpp`: Keyboard input handling.
- `src/render.hpp` / `render.cpp`: Terminal UI drawing and game-over screens.
- `CMakeLists.txt`: Build configuration for the `console_fnaf` executable.
- `SHOWCASE.md`: How to publish for CV, GitHub, and non-technical players.
- `scripts/package-windows.ps1`: Builds and zips a Windows release for GitHub Releases.
- `release/`: `PLAY.bat` and `HOW_TO_PLAY.txt` bundled into the release zip.

## Roadmap

- Add a gameplay GIF or video to this README
- Publish the Windows release zip on GitHub Releases
- Add a title screen and restart flow
- Add more animatronics and difficulty tuning
- Explore a browser-playable version for easier sharing
