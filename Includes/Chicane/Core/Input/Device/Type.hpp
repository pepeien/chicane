#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Input
    {
        enum class DeviceType : std::uint8_t
        {
            Gamepad,
            Keyboard,
            Mouse
        };
    }
}