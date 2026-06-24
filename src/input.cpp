#include "audio.hpp"
#include "input.hpp"

void handleInput(GameState &game, int key)
{
    switch (key)
    {
    case 'a':
    case 'A':
        if (!game.leftDoor)
            playDoorSound();
        game.leftDoor = !game.leftDoor;
        break;

    case 'd':
    case 'D':
        if (!game.rightDoor)
            playDoorSound();
        game.rightDoor = !game.rightDoor;
        break;

    case 'q':
    case 'Q':
        game.running = false;
        break;
    }
}
