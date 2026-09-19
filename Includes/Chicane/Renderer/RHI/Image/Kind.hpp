#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

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

    inline String toString(Renderer::RHI::ImageKind inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::ImageKind::Color2D:
            return "Color2D";

        case Renderer::RHI::ImageKind::Depth2D:
            return "Depth2D";

        case Renderer::RHI::ImageKind::Depth2DArray:
            return "Depth2DArray";

        case Renderer::RHI::ImageKind::Cube:
            return "Cube";

        default:
            return "";
        }
    }
}
