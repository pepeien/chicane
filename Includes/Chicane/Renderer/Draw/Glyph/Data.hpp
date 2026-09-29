#pragma once

#include <vector>

#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Data.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER DrawGlyphData : public DrawData
        {
        public:
            using Points = std::vector<Vec2>;

        public:
            Vec2   boundsMin = Vec2::sZero();
            Vec2   boundsMax = Vec2::sZero();
            Points points    = {};
        };
    }
}
