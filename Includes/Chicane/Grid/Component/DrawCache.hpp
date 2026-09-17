#pragma once

#include <cstdint>

#include "Chicane/Core/Math/Bounds/2D.hpp"
#include "Chicane/Core/Math/Mat/Mat3.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID DrawCache
        {
        public:
            Vec2          position         = Vec2::Zero();
            Mat3          matrix           = Mat3(1.0f);
            Bounds2D      bounds           = {};
            Bounds2D      clip             = Bounds2D::unconstrained();
            std::uint64_t scrollGeneration = 0;
            bool          bIsValid         = false;
        };
    }
}
