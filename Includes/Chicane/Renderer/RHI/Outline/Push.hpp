#pragma once

#include <array>
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER OutlinePush
            {
            public:
                std::array<float, 2> pixelOffset = {};
                std::array<float, 2> padding     = {};
                std::array<float, 4> color       = {};
            };
        }
    }
}
