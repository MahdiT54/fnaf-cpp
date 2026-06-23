#include "ai.hpp"
#include "game_state.hpp"
#include "input.hpp"
#include "render.hpp"

#include <ncurses/curses.h>

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <thread>

int main()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    initTerminalColors();

    std::srand((unsigned)std::time(nullptr));

    GameState game{
        true,  // running
        100,   // battery
        0,     // hoursSurvived
        0.0f,  // hourProgress
        1,     // freddoPos
        1,     // chicoPos
        false, // leftDoor
        false  // rightDoor
    };

    FreddoAI freddo;
    ChicoAI chico;

    bool shouldRedraw = true;
    float batteryAccumulator = 0.0f;

    using clock = std::chrono::steady_clock;
    auto lastTick = clock::now();
    constexpr auto frameDuration{std::chrono::milliseconds(16)}; // ~60 FPS
    auto nextTick = lastTick + frameDuration;
    
// the GAME LOOP //
    while (game.running)
    {
        auto now = clock::now();
        std::chrono::duration<float> delta{now - lastTick};
        float deltaSeconds{delta.count()};
        lastTick = now;

        int ch = getch();
        if (ch != ERR)
        {
            handleInput(game, ch);
            shouldRedraw = true;
        }

        constexpr float SECONDS_PER_HOUR = 20.0f;
        game.hourProgress += deltaSeconds / SECONDS_PER_HOUR;

        if (game.hourProgress >= 1.0f)
        {
            game.hourProgress = 0.0f;
            updateTime(game);
        }

        static float uiTimer = 0.0f;
        uiTimer += deltaSeconds;

        if (uiTimer >= 1.0f)
        {
            shouldRedraw = true;
            uiTimer = 0.0f;
        }

        constexpr float baseDrainPerSecond = 1.0f / 4.0f;
        constexpr float extraPerDoorPerSecond = 0.5f;

        float drainThisSecond = baseDrainPerSecond;
        if (game.leftDoor)
            drainThisSecond += extraPerDoorPerSecond;
        if (game.rightDoor)
            drainThisSecond += extraPerDoorPerSecond;

        batteryAccumulator += drainThisSecond * deltaSeconds;

        while (batteryAccumulator >= 1.0f && game.battery > 0)
        {
            game.battery -= 1;
            batteryAccumulator -= 1.0f;
            shouldRedraw = true;
        }

        freddo.update(game, deltaSeconds);
        chico.update(game, deltaSeconds);

        if (shouldRedraw)
        {
            drawUI(game);
            shouldRedraw = false;
        }

        if (checkGameOver(game))
        {
            break;
        }

        std::this_thread::sleep_until(nextTick);
        nextTick += frameDuration;
        if (clock::now() > nextTick)
            nextTick = clock::now() + frameDuration;
    }

    endwin();
    return 0;
}
