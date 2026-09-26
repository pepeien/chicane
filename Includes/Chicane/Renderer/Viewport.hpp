#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Viewport
        {
        public:
            Vec2 size     = Vec2::sZero();
            Vec2 position = Vec2::sZero();
        };
    }
}