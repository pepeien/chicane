#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER HdrPush
            {
            public:
                std::int32_t enabled = 0;
            };
        }
    }
}
