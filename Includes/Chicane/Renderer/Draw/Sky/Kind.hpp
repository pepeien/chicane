#pragma once

#include <cstdint>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class DrawSkyKind : std::uint8_t
        {
            Cube,
            Panorama
        };
    }
}
