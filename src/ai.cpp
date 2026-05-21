#include "ai.hpp"

#include <algorithm>
#include <cstdlib>

float AnimatronicAI::getAggroRate(const GameState &game) const
{
    return 0.02f * game.hoursSurvived;
}

float AnimatronicAI::getRetreatThreshold() const
{
    return 5.0f - (aggro * 3.0f);
}

float AnimatronicAI::getCooldownDuration() const
{
    return 1.0f + (1.0f - aggro) * 2.0f;
}

void AnimatronicAI::update(GameState &game, float deltaSeconds)
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

        if (blockedTimer >= getRetreatThreshold())
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
        cooldown = getCooldownDuration();
    }
}

bool FreddoAI::isBlocked(const GameState &game) const
{
    return game.leftDoor;
}

void FreddoAI::advance(GameState &game)
{
    if (game.freddoPos < 7)
        game.freddoPos++;
}

void FreddoAI::retreat(GameState &game)
{
    if (game.freddoPos > 1)
        game.freddoPos -= 2;
    if (game.freddoPos < 1)
        game.freddoPos = 1;
}

bool FreddoAI::atDoor(const GameState &game) const
{
    return game.freddoPos == 6;
}

bool ChicoAI::isBlocked(const GameState &game) const
{
    return game.rightDoor;
}

void ChicoAI::advance(GameState &game)
{
    if (game.chicoPos < 8)
        game.chicoPos++;
}

void ChicoAI::retreat(GameState &game)
{
    if (game.chicoPos > 1)
        game.chicoPos -= 2;
    if (game.chicoPos < 1)
        game.chicoPos = 1;
}

bool ChicoAI::atDoor(const GameState &game) const
{
    return game.chicoPos == 7;
}
