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
            enum class LoadOp : std::uint8_t
            {
                Load,
                Clear,
                DontCare
            };
        }
    }

    CHICANE_RENDERER String toString(Renderer::RHI::LoadOp inValue);
}
