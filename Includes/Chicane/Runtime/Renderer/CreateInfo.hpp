#pragma once

#include "Chicane/Runtime/Essential.hpp"

namespace Chicane
{
    struct CHICANE_RUNTIME RendererCreateInfo
    {
    public:
        Vec2 resolution = Vec2::Zero;
        Vec2 position   = Vec2::Zero;
    };
}