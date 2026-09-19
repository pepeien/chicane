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

    inline String toString(Input::DeviceType inValue)
    {
        switch (inValue)
        {
        case Input::DeviceType::Gamepad:
            return "Gamepad";

        case Input::DeviceType::Keyboard:
            return "Keyboard";

        case Input::DeviceType::Mouse:
            return "Mouse";

        default:
            return "";
        }
    }
}
