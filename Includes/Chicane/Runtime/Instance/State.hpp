#pragma once

#include <cstdint>

namespace Chicane
{
    enum class InstanceState : std::uint8_t
    {
        Idle = 0,
        Playing,
        Paused
    };
}
