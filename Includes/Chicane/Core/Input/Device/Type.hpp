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

        inline String toString(DeviceType inValue)
        {
            switch (inValue)
            {
            case DeviceType::Gamepad:
                return "Gamepad";

            case DeviceType::Keyboard:
                return "Keyboard";

            case DeviceType::Mouse:
                return "Mouse";

            default:
                return "";
            }
        }
    }
}