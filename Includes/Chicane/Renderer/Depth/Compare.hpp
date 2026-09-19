#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class DepthCompare : std::uint8_t
        {
            Never,
            Less,
            Equal,
            LessOrEqual,
            Greater,
            NotEqual,
            GreaterOrEqual,
            Always
        };
    }

    inline String toString(Renderer::DepthCompare inValue)
    {
        switch (inValue)
        {
        case Renderer::DepthCompare::Never:
            return "Never";

        case Renderer::DepthCompare::Less:
            return "Less";

        case Renderer::DepthCompare::Equal:
            return "Equal";

        case Renderer::DepthCompare::LessOrEqual:
            return "LessOrEqual";

        case Renderer::DepthCompare::Greater:
            return "Greater";

        case Renderer::DepthCompare::NotEqual:
            return "NotEqual";

        case Renderer::DepthCompare::GreaterOrEqual:
            return "GreaterOrEqual";

        case Renderer::DepthCompare::Always:
            return "Always";

        default:
            return "";
        }
    }
}
