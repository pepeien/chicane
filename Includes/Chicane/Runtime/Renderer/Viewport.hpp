#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Runtime.hpp"

namespace Chicane
{
    struct CHICANE_RUNTIME RendererViewport
    {
    public:
        Vec2 size     = Vec2::Zero;
        Vec2 position = Vec2::Zero;
    };
}