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
            enum class ImageKind : std::uint8_t
            {
                Color2D,
                Depth2D,
                Depth2DArray,
                Cube
            };
        }
    }

    CHICANE_RENDERER String toString(Renderer::RHI::ImageKind inValue);
}
