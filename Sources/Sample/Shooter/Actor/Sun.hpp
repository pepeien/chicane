#pragma once

#include "Chicane/Runtime/Scene/Actor/Light.hpp"

class Sun : public Chicane::ALight
{
public:
    Sun();

private:
    int   m_switch;
    float m_distance;
};