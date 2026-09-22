#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Renderer.hpp"

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

    CHICANE_RENDERER String toString(Renderer::DrawPolyType inValue);
}
