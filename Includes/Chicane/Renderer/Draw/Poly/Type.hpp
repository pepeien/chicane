#pragma once

#include <cstdint>

namespace Chicane
{
    namespace Renderer
    {
        enum class DrawPolyType : std::uint8_t
        {
            e2D,
            e3D,
            eParticle
        };
    }
}