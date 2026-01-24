#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"

namespace Chicane
{
    enum class WindowType : std::uint8_t
    {
        Windowed,
        WindowedBorderless,
        Fullscreen,
    };
}