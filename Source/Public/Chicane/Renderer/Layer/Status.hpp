#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Layer
    {
        enum class Status : std::uint8_t
        {
            Idle,
            Initialized,
            Running
        };
    }
}