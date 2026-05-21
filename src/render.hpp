#pragma once

#include "game_state.hpp"

void initTerminalColors();
void drawUI(const GameState &game);
bool checkGameOver(GameState &game);
