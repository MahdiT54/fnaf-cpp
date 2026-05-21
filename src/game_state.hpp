#pragma once

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

void updateTime(GameState &game);
