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
            enum class SamplerAddress : std::uint8_t
            {
                ClampToEdge,
                ClampToBorder,
                Repeat
            };
        }
    }

    CHICANE_RENDERER String toString(Renderer::RHI::SamplerAddress inValue);
}
