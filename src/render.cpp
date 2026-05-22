#include "render.hpp"

#include <ncurses/curses.h>

#include <chrono>
#include <cstring>
#include <thread>

struct ScreenPos
{
    int row;
    int col;
};

constexpr int MAP_START_ROW = 10;
constexpr int MAP_WIDTH = 28;
constexpr const char *TITLE_LINE = "=====         SURVIVE2SUNRISE        =====";
constexpr int TITLE_WIDTH = 42;

// Positions relative to the map's top-left corner (map drawn at mapCol, MAP_START_ROW).
constexpr ScreenPos ROOM_OFFSETS[] = {
    {0, 0},  // unused (index 0)
    {0, 11}, // [1]
    {2, 1},  // [2]
    {2, 17}, // [3]
    {5, 1},  // [4]
    {5, 23}, // [5]
    {8, 4},  // [6]
    {8, 17}, // [7]
};

constexpr int LEFT_DOOR_COL = 3;
constexpr int RIGHT_DOOR_COL = 16;

constexpr const char *MAP_LINES[] = {
    "          [1]      ",
    "           |       ",
    "[2]-------------[3]",
    " |               | ",
    " |               | ",
    "[4]--------------+----[5]",
    " |               | ",
    " |               | ",
    " --[6]---|YOU|--[7]",
};
constexpr int MAP_HEIGHT = 9;

int mapStartCol()
{
    int col = (TITLE_WIDTH - MAP_WIDTH) / 2;
    return col < 0 ? 0 : col;
}

void initTerminalColors()
{
    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);     // Low battery
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);  // Medium battery
        init_pair(3, COLOR_GREEN, COLOR_BLACK);   // High battery
        init_pair(4, COLOR_RED, COLOR_BLACK);     // Freddo on map
        init_pair(5, COLOR_CYAN, COLOR_BLACK);    // Chico on map
        init_pair(6, COLOR_YELLOW, COLOR_BLACK);  // Closed door indicator
    }
}

void drawEnemy(char symbol, int room, int mapCol)
{
    if (room <= 0 || room >= 8)
        return;

    const ScreenPos &pos = ROOM_OFFSETS[room];
    mvaddch(MAP_START_ROW + pos.row, mapCol + pos.col, symbol);
}

void drawDoorOnMap(int mapCol, int doorCol, char label, bool closed)
{
    int row = MAP_START_ROW + ROOM_OFFSETS[6].row;

    if (closed && has_colors())
        attron(COLOR_PAIR(6) | A_BOLD);

    mvaddch(row, mapCol + doorCol, '[');
    mvaddch(row, mapCol + doorCol + 1, closed ? '#' : label);
    mvaddch(row, mapCol + doorCol + 2, ']');

    if (closed && has_colors())
        attroff(COLOR_PAIR(6) | A_BOLD);
}

void drawDoorStatus(int row, const char *label, bool closed)
{
    mvprintw(row, 0, "%s: ", label);
    const int statusCol = static_cast<int>(std::strlen(label)) + 2;
    const char *status = closed ? "CLOSED" : "OPEN";

    if (closed && has_colors())
        attron(COLOR_PAIR(6) | A_BOLD);

    mvprintw(row, statusCol, "%s", status);

    if (closed && has_colors())
        attroff(COLOR_PAIR(6) | A_BOLD);
}

void drawMap(int mapCol)
{
    for (int i = 0; i < MAP_HEIGHT; ++i)
        mvprintw(MAP_START_ROW + i, mapCol, MAP_LINES[i]);
}

void drawUI(const GameState &game)
{
    clear();
    int displayTime = (12 + game.hoursSurvived) % 12;
    if (displayTime == 0)
        displayTime = 12;

    int seconds = static_cast<int>(game.hourProgress * 60.0f);
    if (seconds >= 60)
        seconds = 59;

    mvprintw(0, 0, "%s", TITLE_LINE);
    mvprintw(1, 0, "=====   Developed by Mahdi Tanzim    =====");
    mvprintw(3, 0, "Time: %02d:%02d AM", displayTime, seconds);

    constexpr int BAR_WIDTH = 20;
    int battery = game.battery;
    if (battery < 0)
        battery = 0;
    if (battery > 100)
        battery = 100;
    int filled = (battery * BAR_WIDTH) / 100;

    int row = 4, col = 0;
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

    drawDoorStatus(5, "Left Door", game.leftDoor);
    drawDoorStatus(6, "Right Door", game.rightDoor);

    const int mapCol = mapStartCol();
    drawMap(mapCol);

    drawDoorOnMap(mapCol, LEFT_DOOR_COL, '6', game.leftDoor);
    drawDoorOnMap(mapCol, RIGHT_DOOR_COL, '7', game.rightDoor);

    mvprintw(20, 0, "Controls: [A] Left | [D] Right | [Q] Quit");

    if (game.freddoPos == game.chicoPos)
    {
        const ScreenPos &pos = ROOM_OFFSETS[game.freddoPos];
        attron(COLOR_PAIR(1));
        mvaddch(MAP_START_ROW + pos.row, mapCol + pos.col, 'X');
        attroff(COLOR_PAIR(1));
    }
    else
    {
        if (useColor)
            attron(COLOR_PAIR(4));
        drawEnemy('F', game.freddoPos, mapCol);
        if (useColor)
            attroff(COLOR_PAIR(4));

        if (useColor)
            attron(COLOR_PAIR(5));
        drawEnemy('C', game.chicoPos, mapCol);
        if (useColor)
            attroff(COLOR_PAIR(5));
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
