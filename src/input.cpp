#include "input.hpp"

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
