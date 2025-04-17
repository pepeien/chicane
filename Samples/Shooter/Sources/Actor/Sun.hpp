#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Game.hpp"

class Sun : public Chicane::ALight
{
public:
    Sun();

private:
    int   m_switch;
    float m_distance;
};