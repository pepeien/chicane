#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace RHI
        {
            enum class PrimitiveTopology : std::uint8_t
            {
                TriangleList,
                TriangleStrip,
                LineList
            };
        }
    }

    inline String toString(Renderer::RHI::PrimitiveTopology inValue)
    {
        switch (inValue)
        {
        case Renderer::RHI::PrimitiveTopology::TriangleList:
            return "TriangleList";

        case Renderer::RHI::PrimitiveTopology::TriangleStrip:
            return "TriangleStrip";

        case Renderer::RHI::PrimitiveTopology::LineList:
            return "LineList";

        default:
            return "";
        }
    }
}
