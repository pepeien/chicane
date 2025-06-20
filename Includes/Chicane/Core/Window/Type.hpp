#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    enum class WindowType : std::uint8_t
    {
        Windowed,
        WindowedBorderless,
        Fullscreen,
    };
}