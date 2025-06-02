#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Window
    {
        enum class Type : std::uint8_t
        {
            Windowed,
            WindowedBorderless,
            Fullscreen,
        };
    }
}