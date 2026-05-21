#include "render.hpp"

#include <ncurses/curses.h>

#include <chrono>
#include <thread>

struct ScreenPos
{
    int row;
    int col;
};

constexpr ScreenPos ROOM_POSITIONS[] = {
    {0, 0},   // unused (index 0)
    {9, 11},  // [1]
    {11, 1},  // [2]
    {11, 17}, // [3]
    {14, 1},  // [4]
    {14, 23}, // [5]
    {17, 4},  // [6] left door
    {17, 17}  // [7] right door
};

void initTerminalColors()
{
    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);    // Low battery
        init_pair(2, COLOR_YELLOW, COLOR_BLACK); // Medium battery
        init_pair(3, COLOR_GREEN, COLOR_BLACK);  // High battery
    }
}

void drawEnemy(char symbol, int room)
{
    if (room <= 0 || room >= 8)
        return;

    const ScreenPos &pos = ROOM_POSITIONS[room];
    mvaddch(pos.row, pos.col, symbol);
}

void drawUI(const GameState &game)
{
    clear();
    // display hour in 12-hour format
    int displayTime = (12 + game.hoursSurvived) % 12;
    if (displayTime == 0)
        displayTime = 12;

    // display seconds
    int seconds = static_cast<int>(game.hourProgress * 60.0f);

    if (seconds >= 60)
        seconds = 59;

    mvprintw(0, 0, "===== FIVE NIGHTS AT FREDDO'S (TEXT) =====");
    mvprintw(1, 0, "=====   Developed by Mahdi Tanzim    =====");
    mvprintw(2, 0, "Time: %02d:%02d AM", displayTime, seconds);

    constexpr int BAR_WIDTH = 20;
    int battery = game.battery;
    if (battery < 0)
        battery = 0;
    if (battery > 100)
        battery = 100;
    int filled = (battery * BAR_WIDTH) / 100;

    int row = 3, col = 0;
    mvprintw(row, col, "Battery: ");
    int barCol = col + 9;
    mvaddch(row, barCol++, '[');

    int colorPair = 3;
    if (battery <= 20)
        colorPair = 1;
    else if (battery <= 50)
        colorPair = 2;

    bool useColor = has_colors();
    if (useColor)
        attron(COLOR_PAIR(colorPair));

    for (int i = 0; i < BAR_WIDTH; ++i)
    {
        if (i < filled)
            mvaddch(row, barCol++, '=');
        else
            mvaddch(row, barCol++, ' ');
    }

    if (useColor)
        attroff(COLOR_PAIR(colorPair));
    mvaddch(row, barCol++, ']');
    mvprintw(row, barCol + 1, " %3d%%", battery);

    mvprintw(4, 0, "Left Door: %s", game.leftDoor ? "CLOSED" : "OPEN");
    mvprintw(5, 0, "Right Door: %s", game.rightDoor ? "CLOSED" : "OPEN");
    mvprintw(9, 0,
             "          [1]      \n"
             "           |       \n"
             "[2]-------------[3]\n"
             " |               | \n"
             " |               | \n"
             "[4]--------------+----[5]\n"
             " |               | \n"
             " |               | \n"
             " --[6]---|YOU|--[7]\n");

    mvprintw(19, 0, "Controls: [A] Left | [D] Right | [Q] Quit");
    if (game.freddoPos == game.chicoPos)
    {
        attron(COLOR_PAIR(1));
        mvaddch(ROOM_POSITIONS[game.freddoPos].row,
                ROOM_POSITIONS[game.freddoPos].col,
                'X');
        attroff(COLOR_PAIR(1));
    }
    else
    {
        attron(COLOR_PAIR(3));
        drawEnemy('F', game.freddoPos);
        attroff(COLOR_PAIR(3));

        attron(COLOR_PAIR(2));
        drawEnemy('C', game.chicoPos);
        attroff(COLOR_PAIR(2));
    }
    refresh();
}

bool checkGameOver(GameState &game)
{
    if (game.battery <= 0)
    {
        clear();
        mvprintw(0, 0, "Battery died.  You sense their approach. (GAME OVER)");
        refresh();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return true;
    }

    if (game.freddoPos == 7 && !game.leftDoor)
    {
        clear();
        mvprintw(0, 0, "Freddo appears at the door. (GAME OVER)");
        refresh();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return true;
    }

    if (game.chicoPos == 8 && !game.rightDoor)
    {
        clear();
        mvprintw(0, 0, "Chico mogged you.  (GAME OVER)");
        refresh();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return true;
    }

    if (game.hoursSurvived >= 6)
    {
        clear();
        mvprintw(0, 0, "It's 6AM, see you in the next shift!");
        refresh();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return true;
    }

    return false;
}
