#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"

class Apple : public Chicane::Actor
{
public:
    Apple();

public:
    void onTick(float inDeltaTime) override;

private:
    bool m_willGoUp;
    float m_upTarget;
};