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
            enum class StencilMode : std::uint8_t
            {
                None,
                WriteReplace,
                TestNotEqual
            };
        }
    }

    CHICANE_RENDERER String toString(Renderer::RHI::StencilMode inValue);
}
