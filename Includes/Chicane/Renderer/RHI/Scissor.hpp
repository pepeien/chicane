#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER Scissor
            {
            public:
                std::int32_t  x      = 0;
                std::int32_t  y      = 0;
                std::uint32_t width  = 0;
                std::uint32_t height = 0;
            };
        }
    }
}
