#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            struct CHICANE_RENDERER MeshPush
            {
            public:
                std::int32_t transparentPass = 0;
            };
        }
    }
}
