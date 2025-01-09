#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Manager
    {
        enum class EventType : std::uint8_t
        {
            Load,
            Allocation,
            Activation
        };
    }
}