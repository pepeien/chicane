#pragma once

#include "Chicane/Box.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class ManagerEvent : std::uint8_t
        {
            Load,
            Allocation,
            Activation,
            Use
        };
    }
}