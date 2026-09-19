#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER Viewport
            {
            public:
                Vec2 size     = Vec2::Zero();
                Vec2 position = Vec2::Zero();
                Vec2 depth    = Vec2(0.0f, 1.0f);
            };
        }
    }
}
