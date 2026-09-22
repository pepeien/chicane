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
            enum class BlendMode : std::uint8_t
            {
                None,
                Alpha,
                Additive
            };
        }
    }

    CHICANE_RENDERER String toString(Renderer::RHI::BlendMode inValue);
}
