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
            enum class SamplerFilter : std::uint8_t
            {
                Nearest,
                Linear
            };
        }
    }

    CHICANE_RENDERER String toString(Renderer::RHI::SamplerFilter inValue);
}
