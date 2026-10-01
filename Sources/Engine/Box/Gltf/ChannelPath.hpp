#pragma once

#include <cstdint>

#include "Chicane/Box.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class ChannelPath : std::uint8_t
        {
            Translation,
            Rotation,
            Scale
        };
    }

    CHICANE_BOX String toString(Box::ChannelPath inValue);
}
