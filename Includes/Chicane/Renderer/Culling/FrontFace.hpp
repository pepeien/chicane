#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Renderer.hpp"

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

    CHICANE_RENDERER String toString(Renderer::CullingFrontFace inValue);
}
