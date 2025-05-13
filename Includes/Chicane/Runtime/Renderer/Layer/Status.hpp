#pragma once

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Layer
    {
        enum class Status : std::uint8_t
        {
            Offline,
            Idle,
            Initialized,
            Running
        };
    }
}