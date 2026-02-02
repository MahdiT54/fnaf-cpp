#include <ncurses/curses.h>
#include <cstdlib>

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
void drawUI(const GameState &game, const FreddoAI &freddo, const ChicoAI &chico);
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
};

struct FreddoAI : AnimatronicAI
{
};

struct ChicoAI : AnimatronicAI
{
};

int main()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

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

    while (game.running)
    {
        freddo.update(game, 0.016f);
        chico.update(game, 0.016f);

        return 0;
    }
};
