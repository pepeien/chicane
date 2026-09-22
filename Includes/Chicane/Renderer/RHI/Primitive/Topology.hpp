#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            enum class PrimitiveTopology : std::uint8_t
            {
                TriangleList,
                TriangleStrip,
                LineList
            };
        }
    }

    CHICANE_RENDERER String toString(Renderer::RHI::PrimitiveTopology inValue);
}
