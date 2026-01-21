#pragma once

#include <map>
#include <vector>

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER Draw2DInstance
        {
        public:
            using List = std::vector<Draw2DInstance>;
            using Map  = std::map<Draw::Id, List>;

        public:
            Vec2  screen   = Vec2::Zero;
            Vec2  size     = Vec2::Zero;
            Vec3  position = Vec3::Zero;

            float _padding[1];
        };
    }
}