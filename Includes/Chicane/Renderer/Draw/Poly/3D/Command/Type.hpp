#pragma once

#include <cstdint>

namespace Chicane
{
    namespace Renderer
    {
        enum class DrawPoly3DCommandType : std::uint8_t
        {
            Undefined,
            Camera,
            Light,
            Sky,
            Mesh
        };
    }
}