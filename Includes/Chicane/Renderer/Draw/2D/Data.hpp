#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawData2D : DrawData
        {
        public:
            Vec2 screen   = Vec2::Zero;
            Vec2 size     = Vec2::Zero;
            Vec3 position = Vec3::Zero;
        };
    }
}