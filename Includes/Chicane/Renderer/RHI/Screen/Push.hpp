#pragma once

#include <array>
#include <cstdint>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER ScreenPush
            {
            public:
                std::array<std::int32_t, 4> header = {};
                std::array<std::int32_t, 4> ids0   = {};
                std::array<std::int32_t, 4> ids1   = {};
            };
        }
    }
}
