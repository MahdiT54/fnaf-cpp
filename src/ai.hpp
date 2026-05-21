#pragma once

#include "game_state.hpp"

struct AnimatronicAI
{
    float aggro = 0.0f;
    float cooldown = 0.0f;
    float decisionAcc = 0.0f;
    float blockedTimer = 0.0f;

    virtual ~AnimatronicAI() = default;

    virtual bool isBlocked(const GameState &game) const = 0;
    virtual void advance(GameState &game) = 0;
    virtual void retreat(GameState &game) = 0;
    virtual bool atDoor(const GameState &game) const = 0;

    virtual float getAggroRate(const GameState &game) const;
    virtual float getRetreatThreshold() const;
    virtual float getCooldownDuration() const;

    void update(GameState &game, float deltaSeconds);
};

struct FreddoAI : AnimatronicAI
{
    bool isBlocked(const GameState &game) const override;
    void advance(GameState &game) override;
    void retreat(GameState &game) override;
    bool atDoor(const GameState &game) const override;
};

struct ChicoAI : AnimatronicAI
{
    bool isBlocked(const GameState &game) const override;
    void advance(GameState &game) override;
    void retreat(GameState &game) override;
    bool atDoor(const GameState &game) const override;
};
