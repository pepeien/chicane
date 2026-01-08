#pragma once

#include "Chicane/Box.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class ManagerEventType : std::uint8_t
        {
            Load,
            Allocation,
            Activation,
            Use
        };
    }
}