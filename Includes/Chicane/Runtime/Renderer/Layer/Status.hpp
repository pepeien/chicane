#pragma once

#include "Core.hpp"

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