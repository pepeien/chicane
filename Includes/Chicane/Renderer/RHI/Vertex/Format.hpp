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
            enum class VertexFormat : std::uint8_t
            {
                Float2,
                Float3,
                Float4
            };
        }
    }

    CHICANE_RENDERER String toString(Renderer::RHI::VertexFormat inValue);
}
