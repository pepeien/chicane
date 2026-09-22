#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

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

    CHICANE_CORE String toString(Input::DeviceType inValue);
}
