#pragma once

#include <cstdint>

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
}