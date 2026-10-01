#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

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

    CHICANE_RENDERER String toString(Renderer::DrawSkyKind inValue);
}
