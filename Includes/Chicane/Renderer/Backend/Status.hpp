#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"
#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        enum class BackendStatus : std::uint8_t
        {
            Running,
            Shutdown
        };
    }

    CHICANE_RENDERER String toString(Renderer::BackendStatus inValue);
}
