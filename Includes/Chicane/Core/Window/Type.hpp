#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    enum class WindowType : std::uint8_t
    {
        Windowed,
        WindowedBorderless,
        Fullscreen,
    };

    inline String toString(WindowType inValue)
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