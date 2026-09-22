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
            enum class BufferUsage : std::uint8_t
            {
                Vertex,
                Index,
                Uniform,
                Storage
            };
        }
    }

    CHICANE_RENDERER String toString(Renderer::RHI::BufferUsage inValue);
}
