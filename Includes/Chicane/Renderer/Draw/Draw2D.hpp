#pragma once

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Draw2D : Draw
        {
        public:
            using List = std::vector<Draw2D>;

        public:
            Vec3 position = Vec3::Zero;
            Vec2 size     = Vec2::Zero;
        };
    }
}