#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class CullingMode : std::uint8_t
        {
            None,
            Front,
            Back,
            FrontAndBack
        };
    }

    CHICANE_RENDERER String toString(Renderer::CullingMode inValue);
}
