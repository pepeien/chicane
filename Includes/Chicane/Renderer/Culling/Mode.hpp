#pragma once

#include <cstdint>

namespace Chicane
{
    namespace Renderer
    {
        enum class CullingMode : std::uint8_t
        {
            None,
            Front,
            Back,
            FrontAndBack
        };
    }
}