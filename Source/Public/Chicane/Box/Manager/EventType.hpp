#pragma once

#include "Chicane/Base.hpp"

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
                Activation
            };
        }
    }
}