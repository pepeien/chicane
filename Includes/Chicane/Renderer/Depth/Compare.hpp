#pragma once

#include <cstdint>

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
}