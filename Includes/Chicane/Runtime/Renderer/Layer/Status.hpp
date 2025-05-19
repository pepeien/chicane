#pragma once

#include "Chicane/Runtime/Essential.hpp"

namespace Chicane
{
    namespace Layer
    {
        enum class Status : std::uint8_t
        {
            Offline,
            Initialized,
            Running
        };
    }
}