#pragma once

#include <Chicane/Runtime/Scene/Actor/Light.hpp>

class Sun : public Chicane::ALight
{
public:
    Sun();

private:
    float m_distance;
};