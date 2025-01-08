#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    enum class WindowType : std::uint8_t
    {
        Windowed,
        WindowedBorderless,
        Fullscreen,
    };
}