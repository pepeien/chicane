#pragma once

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID SvgViewBox
        {
        public:
            // Values
            static constexpr inline float DEFAULT_WIDTH  = 300.0f;
            static constexpr inline float DEFAULT_HEIGHT = 150.0f;

        public:
            Vec2 origin = Vec2::Zero();
            Vec2 size   = Vec2::Zero();
        };
    }
}
