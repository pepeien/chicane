#pragma once

#include <array>
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER BlurPush
            {
            public:
                std::array<float, 2> direction = {};
            };
        }
    }
}
