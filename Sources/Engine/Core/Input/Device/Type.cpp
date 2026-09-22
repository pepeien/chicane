#include "Chicane/Core/Input/Device/Type.hpp"

namespace Chicane
{
    String toString(Input::DeviceType inValue)
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
