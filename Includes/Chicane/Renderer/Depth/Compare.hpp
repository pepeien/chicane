#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Renderer.hpp"

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

    CHICANE_RENDERER String toString(Renderer::DepthCompare inValue);
}
