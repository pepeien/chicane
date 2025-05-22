#pragma once

#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace Manager
        {
            enum class EventType : std::uint8_t
            {
                Load,
                Allocation,
                Activation,
                Use
            };
        }
    }
}