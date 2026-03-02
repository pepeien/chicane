#pragma once

#include <cstdint>

namespace Chicane
{
    namespace Renderer
    {
        enum class CullingFrontFace : std::uint8_t
        {
            CounterClockwise,
            Clockwise
        };
    }
}