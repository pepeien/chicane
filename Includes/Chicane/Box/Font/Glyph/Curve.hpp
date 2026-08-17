#pragma once

#include "Chicane/Box.hpp"

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX FontGlyphCurve
        {
        public:
            using List = std::vector<FontGlyphCurve>;

        public:
            Vec2 start   = Vec2::Zero();
            Vec2 control = Vec2::Zero();
            Vec2 end     = Vec2::Zero();
        };
    }
}