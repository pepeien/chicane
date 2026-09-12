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

        inline String toString(BackendStatus inValue)
        {
            switch (inValue)
            {
            case BackendStatus::Running:
                return "Running";

            case BackendStatus::Shutdown:
                return "Shutdown";

            default:
                return "";
            }
        }
    }
}