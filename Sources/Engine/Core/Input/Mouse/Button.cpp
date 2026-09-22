#include "Chicane/Core/Input/Mouse/Button.hpp"

namespace Chicane
{
    String toString(Input::MouseButton inValue)
    {
        switch (inValue)
        {
        case Input::MouseButton::Left:
            return "Left";

        case Input::MouseButton::Middle:
            return "Middle";

        case Input::MouseButton::Right:
            return "Right";

        default:
            return "";
        }
    }
}
