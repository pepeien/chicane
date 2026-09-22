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
            enum class ImageFormat : std::uint8_t
            {
                RGBA8,
                BGRA8,
                RGBA16F,
                Depth32F,
                Depth24Stencil8
            };
        }
    }

    CHICANE_RENDERER String toString(Renderer::RHI::ImageFormat inValue);
}
