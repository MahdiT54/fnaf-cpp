# Showcase Guide

How to present **Console FNAF** on your CV, GitHub, and social posts — and how to let people try it with minimal friction.

## The core problem

This is a **terminal game**. It uses ncurses, which needs:

1. A real console window (not a background script)
2. Terminal type information (`TERM` + terminfo files)
3. Runtime libraries (on Windows: MSYS2 DLLs)

That means **"clone repo and cmake" is too much** for recruiters, friends, and Twitter followers. Plan for two audiences:

| Audience | Goal | Best approach |
|----------|------|----------------|
| **Viewers** (CV, LinkedIn, GitHub scroll-by) | See that it works in 10 seconds | GIF or short video in README |
| **Players** (actually try it) | Download and play without installing dev tools | GitHub Release zip + `PLAY.bat` |
| **Developers** (clone and hack) | Build from source | README build section |

For a CV, **viewers matter most**. A playable release is a strong bonus; a browser demo is the gold standard if you build one later.

---

## Recommended launch stack (in order)

### 1. Demo media in README (do this first)

Most people will **never run the binary**. They will watch a 15–30 second clip and skim the README.

- Record gameplay in MSYS2 mintty or Windows Terminal (full screen, good contrast).
- Export as GIF (under ~5 MB) or MP4 on YouTube/GitHub.
- Add one static screenshot of the map + UI.
- Put the GIF **above the fold** in `README.md`, before build instructions.

This alone makes the repo CV-ready.

### 2. GitHub Release zip (Windows, non-technical players)

Package a folder users can extract and double-click:

```text
console-fnaf-windows/
  PLAY.bat              ← double-click this
  HOW_TO_PLAY.txt       ← plain-English instructions
  console_fnaf.exe
  libgcc_s_seh-1.dll
  libncursesw6.dll
  libstdc++-6.dll
  libwinpthread-1.dll
  share/terminfo/       ← terminal definitions ncurses needs
```

Create the zip:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/package-windows.ps1
```

Upload `dist/console-fnaf-windows.zip` to a [GitHub Release](https://docs.github.com/en/repositories/releasing-projects-on-github/managing-releases-in-a-repository).

**Player instructions (three steps):**

1. Download `console-fnaf-windows.zip` from Releases.
2. Extract the folder anywhere (Desktop is fine).
3. Double-click `PLAY.bat`. Use `A`, `D`, and `Q` as described in `HOW_TO_PLAY.txt`.

No CMake, compiler, or MSYS2 install required.

### 3. GitHub repo polish (credibility)

- **Description:** one line, e.g. "C++17 terminal FNAF-inspired survival game with ncurses."
- **Topics:** `cpp`, `cmake`, `ncurses`, `terminal-game`, `game-dev`, `learning-project`
- **License:** add `LICENSE` (MIT is common for portfolio projects) before encouraging reuse.
- **Pin the repo** on your GitHub profile if it represents your C++ learning story.

### 4. CV / LinkedIn framing

Keep it outcome-focused:

> **Console FNAF** — C++17 terminal game (CMake, ncurses): real-time loop, game state, virtual AI, win/loss logic.
> [GitHub](link) · [Demo GIF](link) · [Download & play](releases link)

Link the **Release** for "try it" and the **README** for "read the code."

### 5. Browser version (optional, highest reach)

The README roadmap mentions a browser-playable port. That is the **easiest path for fully non-technical users** (single URL, no zip, works on phones).

Options later:

- Rewrite rendering to a web canvas (no ncurses in the browser).
- Emscripten + ported ncurses (heavier, but keeps C++ logic).

Until then, treat the **GIF + Release zip** as your showcase combo.

---

## What does *not* work well for casual players

| Approach | Why it fails |
|----------|----------------|
| "Install MSYS2, pacman, cmake…" | Dev toolchain; most people bounce |
| Raw `.exe` only | Missing DLLs and terminfo → cryptic errors |
| Running from PowerShell/CI | Often missing `TERM` / terminfo |
| Source-only repo | Fine for developers, invisible on a CV |

---

## Pre-launch checklist

### Local verification

- [ ] Build from a clean directory with `cmake -S . -B build`.
- [ ] Run `cmake --build build`.
- [ ] Launch the game from MSYS2/UCRT64 or another ncurses-compatible terminal.
- [ ] Confirm `A`, `D`, and `Q` work.
- [ ] Confirm battery drains over time.
- [ ] Confirm enemies move and game-over conditions trigger.

### GitHub polish

- [ ] Add a short gameplay GIF or video link to `README.md`.
- [ ] Add at least one terminal screenshot.
- [ ] Add a concise project description to the GitHub repo.
- [ ] Add topics such as `cpp`, `cmake`, `ncurses`, `terminal-game`, and `game-dev`.
- [ ] Decide on a license before encouraging reuse.

### Release package

- [ ] Run `scripts/package-windows.ps1` on a clean build.
- [ ] Confirm `dist/console-fnaf-windows.zip` was created.
- [ ] Extract the zip to a fresh folder and double-click `PLAY.bat`.
- [ ] Upload `console-fnaf-windows.zip` to a GitHub Release (tag e.g. `v0.1.0`).
- [ ] Release notes: link to repo, controls (`A` / `D` / `Q`), and "double-click PLAY.bat".
- [ ] Mention Windows SmartScreen may warn because the binary is not code-signed.

### Launch assets

- [ ] Record a 15–30 second gameplay clip (see demo clip checklist below).
- [ ] Post using the social copy below.

---

## Social launch copy

### Core angle

I am a frontend developer learning C++ seriously, so I built a terminal-based survival horror game from scratch.

### Short hook

I have been pushing deeper into C++, so I built a terminal FNAF-inspired game with ncurses: real-time input, battery management, enemy AI, and a full game loop.

### Longer post

I am mostly known as a frontend developer, but I want to build a real future in C++ too.

So I made a small terminal survival game inspired by Five Nights at Freddy's. It uses C++17, CMake, and ncurses. The game has a real-time loop, keyboard input, battery drain, door controls, enemy movement, and win/loss conditions.

This was a fun way to practice the parts of programming that frontend work often hides from you: timing, native builds, terminal rendering, and manually managing game state.

Repo: https://github.com/MahdiT54/fnaf-cpp
Demo: [In progress]

### Technical follow-up ideas

- How the game loop works
- How `std::chrono` powers frame timing
- Why `GameState` is passed by reference
- How virtual functions make the two enemies share behavior
- What ncurses does for terminal rendering
- What was harder coming from frontend development

### Demo clip checklist

- Show the title and UI.
- Toggle the left and right doors.
- Let the battery drain visibly.
- Show Freddo and Chico moving on the map.
- End with a game over or 6 AM win screen.

---

## Next version ideas

- Add a title screen.
- Add restart without closing the program.
- Add sound or text-based jump scare effects.
- Add difficulty levels.
- Create a browser-friendly version for your personal site.

---

## FAQ

**Can macOS/Linux users play the zip?**
No — the packaged zip is Windows-only. macOS/Linux users should build from source (`README.md`); ncurses is usually available via the system package manager.

**Will every Windows user succeed?**
Most Windows 10/11 users with a normal desktop will. Antivirus may flag unknown `.exe` files from GitHub; signing the binary or noting "Windows may warn on first run" in release notes helps.

**Is a video enough for my CV?**
Yes. Many hiring managers skim projects. A clear GIF + concise README + clean code often beats a broken "download and run" experience.
