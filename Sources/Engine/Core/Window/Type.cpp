#include "Chicane/Core/Window/Type.hpp"

namespace Chicane
{
    String toString(WindowType inValue)
    {
        switch (inValue)
        {
        case WindowType::Windowed:
            return "Windowed";

        case WindowType::WindowedBorderless:
            return "WindowedBorderless";

        case WindowType::Fullscreen:
            return "Fullscreen";

        default:
            return "";
        }
    }
}
