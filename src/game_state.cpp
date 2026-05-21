#include "game_state.hpp"

void updateTime(GameState &game)
{
    game.battery -= 1;
    game.hoursSurvived++;
}
