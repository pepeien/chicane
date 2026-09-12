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

        inline String toString(DepthCompare inValue)
        {
            switch (inValue)
            {
            case DepthCompare::Never:
                return "Never";

            case DepthCompare::Less:
                return "Less";

            case DepthCompare::Equal:
                return "Equal";

            case DepthCompare::LessOrEqual:
                return "LessOrEqual";

            case DepthCompare::Greater:
                return "Greater";

            case DepthCompare::NotEqual:
                return "NotEqual";

            case DepthCompare::GreaterOrEqual:
                return "GreaterOrEqual";

            case DepthCompare::Always:
                return "Always";

            default:
                return "";
            }
        }
    }
}