#pragma once

#include <cstdint>

namespace Chicane
{
    enum class InstanceSystem : std::uint8_t
    {
        UI = 0,
        Physics,
        Scene,
        Render,
        Count
    };
}
