#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class DrawPolyMode : std::uint8_t
        {
            Fill,
            Line
        };
    }

    CHICANE_RENDERER String toString(Renderer::DrawPolyMode inValue);
}
