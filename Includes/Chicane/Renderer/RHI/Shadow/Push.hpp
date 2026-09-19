#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER ShadowPush
            {
            public:
                std::uint32_t cascade = 0;
            };
        }
    }
}
