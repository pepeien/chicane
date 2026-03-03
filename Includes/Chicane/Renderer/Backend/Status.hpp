#pragma once

#include <cstdint>

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
}