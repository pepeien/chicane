#pragma once

#include "Chicane/Runtime/Essential.hpp"

namespace Chicane
{
    struct CHICANE_RUNTIME RendererViewport
    {
    public:
        Vec2 size     = Vec2::Zero;
        Vec2 position = Vec2::Zero;
    };
}