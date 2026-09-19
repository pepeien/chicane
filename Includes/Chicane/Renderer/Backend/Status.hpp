#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

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

    inline String toString(Renderer::BackendStatus inValue)
    {
        switch (inValue)
        {
        case Renderer::BackendStatus::Running:
            return "Running";

        case Renderer::BackendStatus::Shutdown:
            return "Shutdown";

        default:
            return "";
        }
    }
}
