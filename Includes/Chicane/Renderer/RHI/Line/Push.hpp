#pragma once

#include <array>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER LinePush
            {
            public:
                std::array<float, 4> color = {1.0f, 1.0f, 1.0f, 1.0f};
                std::array<float, 4> extra = {};
            };
        }
    }
}
