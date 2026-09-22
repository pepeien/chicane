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
            enum class StoreOp : std::uint8_t
            {
                Store,
                DontCare
            };
        }
    }

    CHICANE_RENDERER String toString(Renderer::RHI::StoreOp inValue);
}
