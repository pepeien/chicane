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

    CHICANE_CORE String toString(WindowType inValue);
}