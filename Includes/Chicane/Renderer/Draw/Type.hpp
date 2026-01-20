#pragma once

#include <cstdint>

namespace Chicane
{
    namespace Renderer
    {
        enum class DrawType : std::uint8_t
        {
            TwoDimensional,
            ThreeDimensional,
            Particles
        };
    }
}