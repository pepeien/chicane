#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"

class Sun : public Chicane::Light
{
public:
    Sun();

private:
    float m_radius;
};