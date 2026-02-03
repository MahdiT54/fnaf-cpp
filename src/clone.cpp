#include <ncurses/curses.h>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <thread>

struct GameState
{
    bool running;
    int battery;
    int hoursSurvived;
    float hourProgress;
    int freddoPos;
    int chicoPos;
    bool leftDoor;
    bool rightDoor;
};

void handleInput(GameState &game, int key);
void updateTime(GameState &game);
void drawUI(const GameState &game);
bool checkGameOver(GameState &game);

struct AnimatronicAI
{
    float aggro = 0.0f;
    float cooldown = 0.0f;
    float decisionAcc = 0.0f;
    float blockedTimer = 0.0f;

    virtual bool isBlocked(const GameState &game) const = 0;
    virtual void advance(GameState &game) = 0;
    virtual void retreat(GameState &game) = 0;
    virtual bool atDoor(const GameState &game) const = 0;

    virtual float getAggroRate(const GameState &game) const
    {
        return 0.02f * game.hoursSurvived;
    }

    void update(GameState &game, float deltaSeconds)
    {
        aggro += getAggroRate(game) * deltaSeconds;
        aggro = std::min(aggro, 1.0f);

        if (cooldown > 0.0f)
            cooldown = std::max(0.0f, cooldown - deltaSeconds);

        decisionAcc += deltaSeconds;
        if (decisionAcc < 1.0f)
            return;
        decisionAcc -= 1.0f;

        if (atDoor(game) && isBlocked(game))
        {
            blockedTimer += 1.0f;
            float retreatThreshold = 5.0f - (aggro * 3.0f);

            if (blockedTimer >= retreatThreshold)
            {
                retreat(game);
                blockedTimer = 0.0f;
                cooldown = 2.0f;
                return;
            }
            return;
        }
        else
        {
            blockedTimer = 0.0f;
        }

        if (cooldown > 0.0f)
            return;

        float baseChance = 0.25f + 0.5f * aggro;
        float r = std::rand() / (float)RAND_MAX;

        if (r < baseChance)
        {
            advance(game);
            cooldown = 1.0f + (1.0 - aggro) * 2.0f;
        }
    }
};

struct FreddoAI : AnimatronicAI
{
    bool isBlocked(const GameState &game) const override
    {
        return game.leftDoor;
    }

    void advance(GameState &game) override
    {
        if (game.freddoPos < 7)
            game.freddoPos++;
    }

    void retreat(GameState &game) override
    {
        if (game.freddoPos > 1)
            game.freddoPos -= 2;
        if (game.freddoPos < 1)
            game.freddoPos = 1;
    }

    bool atDoor(const GameState &game) const override
    {
        return game.freddoPos == 6;
    }
};

struct ChicoAI : AnimatronicAI
{
    bool isBlocked(const GameState &game) const override
    {
        return game.rightDoor;
    }

    void advance(GameState &game) override
    {
        if (game.chicoPos < 8)
            game.chicoPos++;
    }

    void retreat(GameState &game) override
    {
        if (game.chicoPos > 1)
            game.chicoPos -= 2;
        if (game.chicoPos < 1)
            game.chicoPos = 1;
    }

    bool atDoor(const GameState &game) const override
    {
        return game.chicoPos == 7;
    }
};

int main()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
    }

    // include <random>
    //  std::random_device rd;
    //  std::mt19937 gen(rd());
    //  std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    //  float r = dist(gen);

    GameState game{
        true,  // running
        100,   // battery
        0,     // hoursSurvived
        0.0f,  // hourProgress
        1,     // freddyPos
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
    constexpr auto frameDuration{std::chrono::milliseconds(16)};
    auto nextTick = lastTick + frameDuration;

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

struct ScreenPos
{
    int row;
    int col;
};

constexpr ScreenPos ROOM_POSITIONS[] =
    {
        {0, 0},
        {9, 11},
        {11, 1},
        {11, 17},
        {14, 1},
        {14, 23},
        {17, 4},
        {17, 17}};

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
    int displayTime = (12 + game.hoursSurvived) % 12;
    if (displayTime == 0)
        displayTime = 12;

    int seconds = static_cast<int>(game.hourProgress * 60.0f);

    if (seconds >= 60)
        seconds = 59;

    mvprintw(0, 0, "Freddo's Game");
    mvprintw(1, 0, "Time: %02d:%02d AM", displayTime, seconds);

    constexpr int Bar_width = 20;
    int battery = game.battery;
    if (battery < 0)
        battery = 0;
    if (battery > 100)
        battery = 100;
    int filled = (battery * Bar_width) / 100;

    int row = 3, col = 0;
    mvprintw(row, col, "Batteria: ");
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

    for (int i = 0; i < Bar_width; ++i)
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
    mvprintw(6, 0, "Freddo's position: %d", game.freddoPos);
    mvprintw(7, 0, "Chico's position: %d", game.chicoPos);
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
        drawEnemy('F', game.freddoPos);
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

void handleInput(GameState &game, int key)
{
    switch (key)
    {
    case 'a':
    case 'A':
        game.leftDoor = !game.leftDoor;
        break;
    case 'd':
    case 'D':
        game.rightDoor = !game.rightDoor;
        break;

    case 'q':
    case 'Q':
        game.running = false;
        break;
    }
}

void updateTime(GameState &game)
{
    game.battery -= 1;
    game.hoursSurvived++;
}

bool checkGameOver(GameState &game)
{
    if (game.battery <= 0)
    {
        clear();
        mvprintw(0, 0, "Battery died.  They're coming. (GAME OVER)");
        refresh();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return true;
    }

    if (game.freddoPos == 7 && !game.leftDoor)
    {
        clear();
        mvprintw(0, 0, "Freddo's here m9, srry (GAME OVER)");
        refresh();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return true;
    }

    if (game.chicoPos == 8 && !game.rightDoor)
    {
        clear();
        mvprintw(0, 0, "Chico lachowski gave you the putskiy.  (GAME OVER)");
        refresh();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return true;
    }

    if (game.hoursSurvived >= 6)
    {
        clear();
        mvprintw(0, 0, "It's 6 o clock, gj");
        refresh();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return true;
    }

    return false;
}
