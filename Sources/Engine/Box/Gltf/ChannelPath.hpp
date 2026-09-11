#pragma once

#include <cstdint>

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
}
