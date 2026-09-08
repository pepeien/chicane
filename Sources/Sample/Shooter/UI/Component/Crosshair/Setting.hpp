#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>

CH_TYPE(Type = (Manual))
struct CrosshairSetting
{
public:
    CrosshairSetting();

public:
    CH_FIELD()
    float size;
    CH_FIELD()
    float thickness;
    CH_FIELD()
    float gap;

    CH_FIELD()
    Chicane::String color;
    CH_FIELD()
    Chicane::String dot;
};
